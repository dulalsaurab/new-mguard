#include "test-common.hpp"

#include <server/data-adapter.hpp>
#include <server/util/stream.hpp>
#include <common.hpp>

#include <ndn-cxx/util/dummy-client-face.hpp>

#include <chrono>
#include <unistd.h>
#include <thread>

using namespace ndn;
using DummyClientFace = ndn::util::DummyClientFace;

namespace mguard {
namespace tests{

BOOST_FIXTURE_TEST_SUITE(TestDataAdapter, IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
 // unit test will be added later
 std::string dbname = "test.db";
 std::string table = "CREATE TABLE gps_episodes_and_semantic_location("  \
                     "ID INT PRIMARY KEY     NOT NULL," \
                     "STARTTIME           DATETIME    NOT NULL," \
                     "ENDTIME            DATETIME     NOT NULL," \
                     "SEMANTIC_NAME        CHAR(50)," \
                     "USER        CHAR(100)," \
                     "VERSION        CHAR(50));";

 DataBase db(dbname, table);

// set(list of semantic names) = get semantic_name from table gps_episodes_and_semantic_location if "2019-09-01 16:40:59" is in between starttime and endtime;
 /* insertion */
// sql = "INSERT INTO COMPANY (ID,STARTTIME,ENDTIME,SEMANTIC_NAME,USER, VERSION) "  \
//      "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
//      "INSERT INTO COMPANY (ID,STARTTIME,ENDTIME,SEMANTIC_NAME,USER, VERSION) "  \
//      "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
//      "INSERT INTO COMPANY (ID,STARTTIME,ENDTIME,SEMANTIC_NAME,USER, VERSION) "  \
//      "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
//      "INSERT INTO COMPANY (ID,STARTTIME,ENDTIME,SEMANTIC_NAME,USER, VERSION) "  \
//      "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
    
    // ,timestamp,localtime,window,semantic_name,user,version
    // ,timestamp,localtime,window,semantic_name,user,version


}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter

} // tests
} // mguard
