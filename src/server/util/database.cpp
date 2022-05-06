#include "database.hpp"

NDN_LOG_INIT(mguard.util.database);

namespace mguard {
namespace db {

DataBase::DataBase(const std::string& databaseName)
: m_databaseName(databaseName), m_db()
{
  // open database if already exist else create a new one
  if (!openDataBase()) {
     NDN_LOG_DEBUG("Failed to open/create to the database");
     exit(-1);
  }
  // this resets the table even if it's already been created
  std::string table = "drop table if exists lookup;create table \
                      lookup(id integer primary key autoincrement, \
                      start integer not null, \
                      end integer not null, \
                      semantic text not null, \
                      user text not null, \
                      version text);";
  
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
    fprintf(stderr, "\n SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return false;
  } else {
    fprintf(stdout, "\n Query exectuted successfully\n");
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

std::vector<std::string>
DataBase::getSemanticLocations(const std::string &timestamp, const std::string &userID) 
{
  // output of function
  std::vector<std::string> out;
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

std::vector<std::string>
DataBase::getRowToInsert(std::string row)
{
  NDN_LOG_TRACE("row to process: " << row);
  std::smatch m;
  std::vector<std::string> result;
  std::regex e ("datetime.datetime\\((.*?)\\)");
  // get start and end timestamp format: YYYYMMDDHHMMSS
  while (std::regex_search(row, m, e)) 
  {  
    std::string _timestamp(m[1]); // e.g. 2019, 9, 1, 23, 34, 59
    boost::trim(_timestamp); // remove whitespaces
    std::vector<std::string> strs;
    boost::split(strs,_timestamp,boost::is_any_of(",")); // create vector from [2019,9,1,23,34,59]
    std::string temp = "";
    for (auto& i: strs) {
      boost::trim(i);
      if (i.size() < 2)
        i = "0"+i;
      temp += i; // concetenate to obtain 20190901233459 from 2019,9,1,23,34,59
    }

    NDN_LOG_TRACE("timestamp: " << temp);
    result.push_back(temp);
    row = m.suffix();
  }
  NDN_LOG_TRACE("remaining: " << row); // remaining will be e.g. )",shopping-mall,dd40c,1
  std::vector<std::string> _t;
  boost::split(_t,row,boost::is_any_of(","));
  result.insert(result.end(), {_t[1],_t[2],_t[3]});

  return result;
}

void
DataBase::insertRows(const std::vector<std::string>& dataSet)
{
  if (!openDataBase()) {
    NDN_LOG_INFO("Couldn't open database");
    return;
  }
  
  std::string query = "INSERT INTO lookup (start, end, semantic, user, version) VALUES";
  std::string value = "";
  // for (auto& row : dataSet)
  for (auto it = dataSet.begin(); it != dataSet.end(); ++it)
  {
    NDN_LOG_TRACE("data point: " << *it);
    //TODO: check if row is empty; also populating the values cane be better
    try {
      auto pRow = getRowToInsert(*it); //processed row

      if (pRow.size() < 5) // don't have all the required element, skip the insertion
        continue;

      value += "(";
      value += "\""+ pRow[0] +"\"" + ","; // start time
      value += "\""+ pRow[1] +"\"" + ","; // end time
      value += "\""+ pRow[2] +"\"" + ","; // semantic location
      value += "\""+ pRow[3] +"\"" + ","; // user
      value += "\""+ pRow[4] +"\""; // version
      value += ")";
      if (!(std::next(it) == dataSet.end()))
        value += ",";
    }
    catch (const std::exception& ex)
    {
      NDN_LOG_DEBUG("couldn't process the row: " << *it);
      NDN_LOG_ERROR("error: " << ex.what());
      continue;
    }
  }
  query += value;

  NDN_LOG_TRACE("Insert query: " <<  query);

  if (runQuery(query))
    NDN_LOG_DEBUG("Inserted all the rows successfully");
  else
    NDN_LOG_DEBUG("Failed to insert the rows");

  closeDataBase();
}

} // db
} // mguard