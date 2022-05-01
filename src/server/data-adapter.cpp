#include "common.hpp"
#include "data-adapter.hpp"

#include <iostream>
#include <string>

// using namespace boost::placeholders;

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard {

ConnectionHandler::ConnectionHandler(boost::asio::io_service& io_service,
                                     const CallbackFromController& callback)
: sock(io_service)
, m_onReceiveDataFromClient(callback)
{
}

void 
ConnectionHandler::start()
{
  // this will read the header
  sock.async_read_some(
      boost::asio::buffer(data, max_length),
      boost::bind(&ConnectionHandler::readContent,
                  shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

  sock.async_write_some(
      boost::asio::buffer(message, max_length),
      boost::bind(&ConnectionHandler::writeHandle,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void
ConnectionHandler::readContent(const boost::system::error_code& err, size_t bytes_transferred)
{
  std::cout << "Header received from the socket " << data << std::endl;
  boost::split(metaData, data, boost::is_any_of("|"));
  std::cout << "expected number of chunks: " << getExpectedNumberOfChunks() << std::endl;
  
  // todo: need to replace this function, read exact number of bytes as expected 
  async_read(sock, response_,
             boost::bind(&ConnectionHandler::readHandle, 
             shared_from_this(),
             boost::asio::placeholders::error,
             boost::asio::placeholders::bytes_transferred));
}

void 
ConnectionHandler::readHandle(const boost::system::error_code& err, size_t bytes_transferred)
{
  NDN_LOG_DEBUG("Total number of bytes recevied: " << bytes_transferred);
  auto expectedBytes = static_cast<size_t> (std::stoi(metaData[2]));

  if (response_.size() <= 0) { // didn't get anything
      NDN_LOG_DEBUG("Missing content");
      std::vector<std::string> emptyVector= {};
      m_onReceiveDataFromClient(emptyVector, ""); // problem receiving content, don't think we need to send callback here??
    }
  
  if (response_.size() != expectedBytes) { // didn't get all the expected bytes  
    NDN_LOG_DEBUG("All the expected data is not received");
  }

  // todo: error wont occure once the above function is fixed
  if (err)
    NDN_LOG_DEBUG("Error: " << err);
  
  // convert the buffer into string and send it to server
  std::string resp ((std::istreambuf_iterator<char>(&response_)), std::istreambuf_iterator<char>());
  m_onReceiveDataFromClient(metaData, resp);
  sock.close();
}

void 
ConnectionHandler::writeHandle(const boost::system::error_code& err, size_t bytes_transferred)
{
  if (!err) {
      cout << "Server sent Hello message!"<< endl;
  } else {
      std::cerr << "error: " << err.message() << endl;
      sock.close();
  }
}

Receiver::Receiver(boost::asio::io_service& io_service, const CallbackFromReceiver& callback)
: acceptor_(io_service, tcp::endpoint(tcp::v4(), 8808))
, m_onReceiveDataFromController(callback)
{
  startAccept();
}

void 
Receiver::startAccept()
{
  ConnectionHandler::pointer connection = ConnectionHandler::create(GET_IO_SERVICE(acceptor_), 
                                           std::bind(&Receiver::processCallbackFromController, this, _1, _2));

  acceptor_.async_accept(connection->socket(),
                         boost::bind(&Receiver::handleAccept, this, connection,
                         boost::asio::placeholders::error));
}


void
Receiver::processCallbackFromController(const std::vector<std::string> metaData, const std::string& response)
{
  // check if the metaData is empty, and there is no response
  if (!(response.empty()))
    m_onReceiveDataFromController(metaData[0], response);

  // do nothing
  NDN_LOG_DEBUG("Didn't receive any data from the receiver");
}


void 
Receiver::handleAccept(ConnectionHandler::pointer connection, const boost::system::error_code& err)
{
  if (!err) {
    connection->start();
  }
  startAccept();
}

DataAdapter::DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix,
                         const ndn::Name& aaPrefix, const std::string& lookupDatabase)
: m_face(face)
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity(producerPrefix).getDefaultKey().getDefaultCertificate())
, m_ABE_authorityCert(m_keyChain.getPib().getIdentity(aaPrefix).getDefaultKey().getDefaultCertificate())
, m_publisher(m_face, m_keyChain, m_producerPrefix, m_producerCert, m_ABE_authorityCert)
, m_receiver(m_face.getIoService(), 
            std::bind(&DataAdapter::processCallbackFromReceiver, this, _1, _2))
, m_dataBase(lookupDatabase)
{
  NDN_LOG_DEBUG ("Initialized data adaptor and publisher");
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("Producer cert: " << m_producerCert);
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("ABE authority cert: " << m_ABE_authorityCert);
}

void
DataAdapter::processCallbackFromReceiver(const std::string& streamName, const std::string& streamContent)
{
  NDN_LOG_DEBUG("Received data from the receiver"); 
  auto content = m_fileProcessor.getVectorByDelimiter(streamContent, "\n");
  if (streamName == SEMANTIC_LOCATION) {
    // insert the data into the lookup table
    NDN_LOG_DEBUG("Received semantic location data");
    m_dataBase.insertRows(content);
  }

  // TODO: ---> streamName, streamName ?
  m_streams.emplace(streamName, streamName);
  publishDataUnit(m_streams.find(streamName)->second, content);
}

void
DataAdapter::run()
{
  try {
    m_face.processEvents(); // block mode
    // face1.getIoService().run(); //go in block mode sync --- (interest/waits for data)

  }
  catch (const std::exception& ex)
  {
    NDN_LOG_ERROR("Face error: " << ex.what()); 
    NDN_THROW(Error(ex.what()));
  }
}

void
DataAdapter::stop()
{
  NDN_LOG_DEBUG("Shutting down face: ");
  m_face.shutdown();
}

ndn::Name
DataAdapter::makeDataName(ndn::Name streamName, std::string timestamp)
{
  NDN_LOG_TRACE("Creating data name from streamName: " << streamName << "and timestamp: " << timestamp);
  return streamName.append("DATA").append(timestamp);
}

void
DataAdapter::publishDataUnit(util::Stream& stream, const std::vector<std::string>& dataSet)
{
  NDN_LOG_INFO("Processing stream: " << stream.getName());

  for (auto data : dataSet)
  {
    // get timestamp from the data row
    std::string delimiter = ",";
    m_tempRow = data;
    auto timestamp = m_tempRow.substr(0, m_tempRow.find(delimiter));
    auto dataName = makeDataName(stream.getName(), timestamp);
    NDN_LOG_DEBUG ("Publishing data name: " << dataName << " Timestamp: " << timestamp);

    //TODO: need to change this, don't want to pass stream here, but rather just the attributes.
    m_publisher.publish(dataName, data, stream);
  }
}

} //mguard