#include "common.hpp"
#include "data-adapter.hpp"

#include <iostream>
#include <string>


NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard {

ConnectionHandler::ConnectionHandler(boost::asio::io_service& io_service)
: sock(io_service)
{
}

void 
ConnectionHandler::start()
{
  //  async_read(sock, response_,
  //     boost::asio::transfer_at_least(10),
  //     boost::bind(&ConnectionHandler::readHandle, this,
  //     boost::asio::placeholders::error));
        
  sock.async_read_some(
      boost::asio::buffer(data, max_length),
      boost::bind(&ConnectionHandler::readHandle,
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
ConnectionHandler::readHandle(const boost::system::error_code& err, size_t bytes_transferred)
{

  if (!err)
    {
      // Write all of the data that has been read so far.
      // std::cout << response_;
      // Continue reading remaining data until EOF.
      std::cout << bytes_transferred << std::endl;
      // std::cout << data << std::endl;
      sock.async_read_some(
      boost::asio::buffer(data, max_length),
      boost::bind(&ConnectionHandler::readHandle,
                  shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

      // // boost::asio::async_read(sock, response_,
      //     boost::asio::transfer_at_least(1),
      //     boost::bind(&ConnectionHandler::readHandle, this,
      //       boost::asio::placeholders::error,
      //       boost::asio::placeholders::bytes_transferred));
    }
    else if (err != boost::asio::error::eof)
    {
      std::cout << "Error: " << err << "\n";
       sock.close();
    }

    // std::cout << data << std::endl;


  // std::cout << "did i come here???" << std::endl;
  // if (!err) {
  //       cout << data << endl;
  // } else {
  //       std::cerr << "error: " << err.message() << std::endl;
  //       sock.close();
  // }
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

Server::Server(boost::asio::io_service& io_service)
: acceptor_(io_service, tcp::endpoint(tcp::v4(), 8808))
{
  start_accept();
}

void 
Server::start_accept()
{
  ConnectionHandler::pointer connection = ConnectionHandler::create(GET_IO_SERVICE(acceptor_));

  acceptor_.async_accept(connection->socket(),
                         boost::bind(&Server::handle_accept, this, connection,
                         boost::asio::placeholders::error));
}

void 
Server::handle_accept(ConnectionHandler::pointer connection, const boost::system::error_code& err)
{
  if (!err) {
    connection->start();
  }
  start_accept();
}

DataAdapter::DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix, const ndn::Name& aaPrefix)
: m_face(face)
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity(producerPrefix).getDefaultKey().getDefaultCertificate())
, m_ABE_authorityCert(m_keyChain.getPib().getIdentity(aaPrefix).getDefaultKey().getDefaultCertificate())
, m_publisher(m_face, m_keyChain, m_producerPrefix, m_producerCert, m_ABE_authorityCert)
, m_server(m_face.getIoService())
{
  NDN_LOG_DEBUG ("Initialized data adaptor and publisher");
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("Producer cert: " << m_producerCert);
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("ABE authority cert: " << m_ABE_authorityCert);
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
  return streamName.append("DATA").append(timestamp);
}

void
DataAdapter::publishDataUnit(util::Stream& stream)
{
  NDN_LOG_INFO("Processing stream: " << stream.getName());
  auto dataSet = m_fileProcessor.readStream(stream.getStreamDataPath());

  for (auto data : dataSet)
  {
    NDN_LOG_TRACE("Data unit" << data);
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

DataBase::DataBase(const std::string& databaseName, const std::string& table)
: m_databaseName(databaseName)
{
  // open database if already exist else create a new one
  if (!openDataBase()) {
    NDN_LOG_DEBUG("Failed to open/create to the database");
    exit(-1);
  }
  if (!runQuery(table)) {
    NDN_LOG_INFO("Failed to create table");
    exit(-1);
  }
  NDN_LOG_DEBUG("Database and table crated successfully");
  closeDataBase();
}

inline bool
DataBase::openDataBase()
{
  auto errStatus =  sqlite3_open(m_databaseName.c_str(), &m_db);
  if (errStatus) {
    std::cerr << "Error open DB " << sqlite3_errmsg(m_db) << std::endl;
    return false;
  }
  else
    NDN_LOG_INFO("Opened Database Successfully!");
  return true;
}

inline
void
DataBase::closeDataBase()
{
  sqlite3_close(m_db);
}

bool
DataBase::runQuery(const std::string& query)
{
  char *zErrMsg = 0;
  /* Execute SQL statement */
  auto rc = sqlite3_exec(m_db, query.c_str(), nullptr, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return false;
   } else {
      fprintf(stdout, "Query exectuted successfully\n");
   }
   return true;
}

void
DataBase::insertRows(const std::vector<std::string>& rows)
{
  if (!openDataBase()) {
    NDN_LOG_INFO("Couldn't open database");
    return;
  }
  
  for (const auto &row : rows) {
    if (!runQuery(row))
      NDN_LOG_DEBUG("Failed to insert row: " << row);

    NDN_LOG_DEBUG("Inserted row: " << row);
  }
  closeDataBase();
}


} //mguard