#ifndef MGUARD_DATA_ADAPTER_HPP
#define MGUARD_DATA_ADAPTER_HPP

#include "publisher.hpp"
#include "file-processor.hpp"
#include "util/stream.hpp"

#include <PSync/full-producer.hpp>
#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <unordered_map>
#include <sqlite3.h> 
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

//importing libraries
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

#if BOOST_VERSION >= 107000
#define GET_IO_SERVICE(s) ((boost::asio::io_context&)(s).get_executor().context())
#else
#define GET_IO_SERVICE(s) ((s).get_io_service())
#endif


namespace mguard {

class ConnectionHandler : public boost::enable_shared_from_this<ConnectionHandler>
{
private:
  tcp::socket sock;
  std::string message="Hello From Server!";
  enum { max_length = 1024 };
  char data[max_length];
  boost::asio::streambuf response_;

public:
  
  typedef boost::shared_ptr<ConnectionHandler> pointer;

  ConnectionHandler(boost::asio::io_service& io_service);
  
  // creating the pointer
  static pointer 
  create(boost::asio::io_service& io_service)
  {
    return pointer(new ConnectionHandler(io_service));
  }

  //socket creation
  tcp::socket& 
  socket()
  {
    return sock;
  }
  
  void 
  start();

  void 
  readHandle(const boost::system::error_code& err, size_t bytes_transferred);

  void 
  writeHandle(const boost::system::error_code& err, size_t bytes_transferred);

};

class Server 
{
private:
   tcp::acceptor acceptor_;

public:
  
  void 
  start_accept();

  Server(boost::asio::io_service& io_service);

  void
  handle_accept(ConnectionHandler::pointer connection, const boost::system::error_code& err);

};

class DataAdapter
{

public:
  DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix, const ndn::Name& aaPrefix);
  
  void
  run();

  void
  stop();

  ndn::Name
  makeDataName(ndn::Name streamName, std::string timestamp);
  
  void
  publishDataUnit(util::Stream& stream);

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
  mguard::Server m_server;
};

class DataBase
{
public:
    static int
    callback (void *NotUsed, int argc, char **argv, char **azColName);

    DataBase(std::basic_string<char>  databaseName);

    sqlite3*
    getDatabase()
    {
        return m_db;
    }

    void
    closeDataBase();

    bool
    openDataBase();

    // main function that gets the unique semantic locations from the db given a timestamp and userID
    // call this after the database is populated, or else it won't work
    // timestamp is in the format YYYYMMDDHHMMSS
    std::vector<std::basic_string<char>>
    getLocations( std::basic_string<char> &timestamp, std::basic_string<char> &userID);

    void
    insertRows(std::vector<std::vector<std::basic_string<char>>>& rows);

    void
    deleteRows(std::string deleteQuery);

    bool
    runQuery(const std::string& query);

    // takes csv string
    // converts into 2d vector of rows with values
    static std::vector<std::vector<std::basic_string<char>>>
    processCSV(std::basic_string<char>& _s);

    // this is how I got the two dates from a string that looked like this:
    // Row(_1=datetime.datetime(2019, 9, 1, 11, 34, 59), _2=datetime.datetime(2019, 9, 1, 13, 34, 59))
    // which returns [2019901113459, 20190901133459] given that example
    static std::vector<std::string>
    dateThing(const std::basic_string<char>& unformattedDate);

private:
    sqlite3* m_db;
    // std::shared_ptr<sqlite3*> m_db;
    std::string m_databaseName;

};



} // mguard

#endif // MGUARD_DATA_ADAPTER_HPP