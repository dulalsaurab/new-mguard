#ifndef MGUARD_DATA_ADAPTER_HPP
#define MGUARD_DATA_ADAPTER_HPP

#include "publisher.hpp"
#include "file-processor.hpp"
#include "util/stream.hpp"
#include "util/database.hpp"

#include <PSync/full-producer.hpp>
#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <unordered_map>
#include <sqlite3.h> 
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

#define BOOST_BIND_NO_PLACEHOLDERS

#if BOOST_VERSION >= 107000
#define GET_IO_SERVICE(s) ((boost::asio::io_context&)(s).get_executor().context())
#else
#define GET_IO_SERVICE(s) ((s).get_io_service())
#endif


namespace mguard {

using CallbackFromController = std::function<void(const std::vector<std::string> metaData, const std::string response)>;
using CallbackFromReceiver = std::function<void(const std::string& streamName, const std::string& streamContent)>;

class ConnectionHandler : public boost::enable_shared_from_this<ConnectionHandler>
{
public:
  
  typedef boost::shared_ptr<ConnectionHandler> pointer;

  ConnectionHandler(boost::asio::io_service& io_service, const CallbackFromController& callback);
  
  // creating the pointer
  static pointer 
  create(boost::asio::io_service& io_service, const CallbackFromController& callback)
  {
    return pointer(new ConnectionHandler(io_service, callback));
  }

  //socket creation
  tcp::socket& 
  socket()
  {
    return sock;
  }
  
  inline int
  getExpectedNumberOfChunks()
  {
    if(!(metaData.size() > 0))
      return -1;
    return std::stoi(metaData[1]);
  }

  void 
  start();

  // void
  // readHeader(const boost::system::error_code& err, size_t bytes_transferred);

  void
  readContent(const boost::system::error_code& err, size_t bytes_transferred);
  
  void
  readHandle(const boost::system::error_code& err, size_t bytes_transferred);

  void 
  writeHandle(const boost::system::error_code& err, size_t bytes_transferred);

  void
  resetData()
  {
    // data[max_length] = {};
    memset(data, 0, sizeof(data));
  }

private:
  tcp::socket sock;
  std::string message="Hello From Server!";
  
  enum { max_length = 1024 };
  char data[max_length];
  
  std::vector<std::string> metaData;
  boost::asio::streambuf response_;
  CallbackFromController m_onReceiveDataFromClient;

};

class Receiver 
{
public:
  
  Receiver(boost::asio::io_service& io_service, const CallbackFromReceiver& callback);

  void 
  startAccept();

  void
  processCallbackFromController(const std::vector<std::string> metaData, const std::string& response);

  void
  handleAccept(ConnectionHandler::pointer connection, const boost::system::error_code& err);

private:
   tcp::acceptor acceptor_;
   CallbackFromReceiver m_onReceiveDataFromController;

};

class DataAdapter
{

public:

  DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix,
                         const std::string& producerCertPath,
                         const ndn::Name& aaPrefix, const std::string& aaCertPath,
                         const std::string& lookupDatabase);
  
  void
  run();

  void
  stop();

  void
  processCallbackFromReceiver(const std::string& streamName, const std::string& streamContent);

  ndn::Name
  makeDataName(ndn::Name streamName, std::string timestamp);
  
  void
  publishDataUnit(util::Stream& stream, const std::vector<std::string>& dataSet);

private:
  ndn::KeyChain m_keyChain;
  ndn::Face& m_face;

  FileProcessor m_fileProcessor;
  std::string m_tempRow;
  ndn::Name m_producerPrefix;

  ndn::security::Certificate m_producerCert;
  ndn::security::Certificate m_ABE_authorityCert;

  mguard::Publisher m_publisher;
  boost::asio::io_service m_ioService;
  mguard::Receiver m_receiver;
  std::map<std::string, mguard::util::Stream> m_streams;
  db::DataBase m_dataBase;

};

} // mguard

#endif // MGUARD_DATA_ADAPTER_HPP