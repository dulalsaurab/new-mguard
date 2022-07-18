#ifndef MGUARD_DATABASE_HPP
#define MGUARD_DATABASE_HPP

// #include "file-processor.hpp"
#include "stream.hpp"

#include <ndn-cxx/util/logger.hpp>

#include <sqlite3.h> 
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

namespace mguard {
namespace db {

class DataBase
{
public:

  DataBase(const std::string& databaseName);

  static int 
  callback(void *NotUsed, int argc, char **argv, char **azColName);

  sqlite3*
  getDatabase()
  {
    return m_db;
  }

  inline
  void
  closeDataBase() 
  {
    sqlite3_close(m_db);
  }

  bool
  openDataBase();
  /* 
    main function that gets the unique semantic locations from the db given a timestamp and userID
    call this after the database is populated, or else it won't work timestamp is in the format YYYYMMDDHHMMSS
  */
  std::vector<std::string>
  getSemanticLocations(const std::string& timestamp, const std::string& userID);

  std::vector<std::string>
  getRowToInsert(std::string row);

  void
  insertRows(const std::vector<std::string>& dataSet);

  // not implemented??
  void
  deleteRows(std::string deleteQuery);

  bool
  runQuery(const std::string& query);

private:
  const std::string& m_databaseName;
  sqlite3* m_db;
};

} // db
} // mguard

#endif // MGUARD_DATABASE_HPP
