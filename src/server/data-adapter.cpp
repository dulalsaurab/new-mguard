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

DataAdapter::DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix, const ndn::Name& aaPrefix)
: m_face(face)
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity(producerPrefix).getDefaultKey().getDefaultCertificate())
, m_ABE_authorityCert(m_keyChain.getPib().getIdentity(aaPrefix).getDefaultKey().getDefaultCertificate())
, m_publisher(m_face, m_keyChain, m_producerPrefix, m_producerCert, m_ABE_authorityCert)
, m_receiver(m_face.getIoService(), 
           std::bind(&DataAdapter::processCallbackFromReceiver, this, _1, _2))
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
  if (streamName == SEMANTIC_LOCATION) { 
    // insert the data into the lookup table
  }

  // TODO: ---> streamName, streamName ?
  m_streams.emplace(streamName, streamName);
  publishDataUnit(m_streams.find(streamName)->second, streamContent);
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
DataAdapter::publishDataUnit(util::Stream& stream, const std::string& streamContent)
{
  NDN_LOG_INFO("Processing stream: " << stream.getName());
  auto dataSet = m_fileProcessor.getVectorByDelimiter(streamContent, "\n");

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

DataBase::DataBase(std::basic_string<char>  databaseName)
: m_databaseName(std::move(databaseName)), m_db()
{
  // open database if already exist else create a new one
  if (!openDataBase()) {
     NDN_LOG_DEBUG("Failed to open/create to the database");
     exit(-1);
  }
  // this resets the table even if it's already been created
  std::string table = "drop table if exists lookup;create table lookup(id integer primary key autoincrement, start integer not null, end integer not null, semantic text not null, user text not null, version text);";
  if (!runQuery(table)) 
  {
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

bool
DataBase::runQuery(const std::string& query)
{
  char *zErrMsg = nullptr;
  /* Execute SQL statement */
  std::basic_string<char> data;
  auto rc = sqlite3_exec(m_db, query.c_str(), callback, &data, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return false;
  } else {
    fprintf(stdout, "Query exectuted successfully\n");
  }
  return true;
}

int
DataBase::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i = 0; i<argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

std::vector<std::basic_string<char>>
DataBase::getSemanticLocations(std::basic_string<char> &timestamp, std::basic_string<char> &userID) 
{
  // output of function
  std::vector<std::basic_string<char>> out;
  // creating the query based on given timestamp and userID
  std::string tmpQuery = "select distinct semantic from lookup where start <= ";
  tmpQuery += timestamp;
  tmpQuery += " and end >= ";
  tmpQuery += timestamp;
  tmpQuery += ";";

  // open database, run query on database, and store result in stmt
  openDataBase();
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(m_db,tmpQuery.c_str(), -1, &stmt, nullptr);

  // variables for storing values from each row
  const char* tmp;    // raw from row
  char *safe;         // will be safe spot in memory
  while (sqlite3_step(stmt) != SQLITE_DONE){
    // get the next value in column index 0 of the query's result
    tmp = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    // set a new place in memory to the size needed for new value
    safe = new char[strlen(tmp)];
    // copy value from tmp to safe memory
    strcpy(safe, tmp);

    // add locatoin of safe value to output
    out.emplace_back(safe);
  }

  // close stmt and database
  sqlite3_finalize(stmt);
  closeDataBase();

  return out;
}

std::vector<std::vector<std::basic_string<char>>>
DataBase::processCSV(std::basic_string<char> &_s) 
{
  std::vector<std::vector<std::basic_string<char>>> rows;
  std::vector<std::basic_string<char>> row;
  std::basic_string<char> value;
  bool inQuote = false;
  for (const char &c: _s) 
  {
    if (inQuote) {
      if (c == '"') {
        inQuote = false;
      } else {
      value.push_back(c);
      }
    }
    else
    {
      // start of quote
      if (c == '"') {
        inQuote = true;
      }
        // end of item
      else if (c == ',') {
        row.push_back(value);
        value.clear();
      }
      // is escape character
      else if (c == '\n') {
        row.push_back(value);
        value.clear();

        rows.push_back(row);
        row.clear();
      }
        // part of item
      else {
        value.push_back(c);
      }
    }
    }
    if (!value.empty()){
      row.push_back(value);
      value.clear();
    }
    if (!row.empty()) {
      rows.push_back(row);
      row.clear();
    }
    return rows;
}

void
DataBase::insertRows(std::vector<std::vector<std::basic_string<char>>>& rows)
{
  if (!openDataBase()) {
    NDN_LOG_INFO("Couldn't open database");
    return;
  }

  std::string query;
  for (const std::vector<std::basic_string<char>>& row : rows) {
    query = "insert into lookup (start, end, semantic, user, version) values(";
    std::vector<std::string> x = getStartEndDate(row[3]);
    // header line
    if (x.empty()) {
      continue;
    }
    for (const auto &item: x) {
      query += item;
      query += ",";
    }
    query += "\"" + row[4] + "\"";
    query += ", ";
    query += "\"" + row[5] + "\"";
    query += ", ";
    query += "\"" + row[6] + "\"";
    query += ")";

    if (runQuery(query)) {
      NDN_LOG_DEBUG("Failed to insert the row");
    } else {
      NDN_LOG_DEBUG("Inserted row");
    }
    }
    closeDataBase();
}

// you can override this to add any formatting you want
// CHANGING THE FORMATTING FOR THIS WILL RESULT IN INCORRECT QUERIES
// currently, this produces YYYYMMDDHHMMSS
std::vector<std::string>
DataBase::getStartEndDate(const std::basic_string<char>& unformattedDate) 
{
  std::vector<std::string> out;
  // full is the building of the full YYYYMMDDHHMMSS number
  // individual is the YYYY or MM or DD, etc, where it needs to be 2 or 4 digits long for formatting
  std::basic_string<char> full, individual;
  for (const char &c: unformattedDate) {
    if (c == '('){
      full.clear();
      individual.clear();
    }
    else if (c == ')'){
      if (!individual.empty()) {
        // this is where you can add formatting like - or : (in an overridden function)
        full += individual;
        individual.clear();
      }
      if (!full.empty()){
        out.push_back(full);
      }
      full.clear();
      individual.clear();
    }
    else if (c == ',' ) {
      if (individual.size() < 2) {
        full += "0";
      }
      full += individual;
      individual.clear();
    }
    else if (c != ' ') {
      individual.push_back(c);
    }
  }
  return out;
}

} //mguard