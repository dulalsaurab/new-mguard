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
    std::string dbname = "test.db";

    // simulating csv that the socket should've gotten
    std::string from_socket = ",timestamp,localtime,window,semantic_name,user,version\n"
    "0,2019-09-01 18:34:59,2019-09-01 23:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 11, 34, 59), _2=datetime.datetime(2019, 9, 1, 13, 34, 59))\",shopping-mall,dd40c,1\n"
    "1,2019-09-01 18:34:59,2019-09-01 23:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 11, 34, 59), _2=datetime.datetime(2019, 9, 1, 13, 34, 59))\",moving-mall,dd40c,1\n"
    "2,2019-09-01 18:34:59,2019-09-01 23:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 11, 34, 59), _2=datetime.datetime(2019, 9, 1, 13, 34, 59))\",moving-mall,dd40c,1\n"
    "3,2019-09-02 00:34:59,2019-09-02 05:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 17, 34, 59), _2=datetime.datetime(2019, 9, 1, 19, 34, 59))\",moving-mall,dd40c,1\n"
    "4,2019-09-02 00:34:59,2019-09-02 05:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 17, 34, 59), _2=datetime.datetime(2019, 9, 1, 19, 34, 59))\",moving-mall,dd40c,1\n"
    "5,2019-09-02 00:34:59,2019-09-02 05:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 17, 34, 59), _2=datetime.datetime(2019, 9, 1, 19, 34, 59))\",moving-mall,dd40c,1\n"
    "6,2019-09-02 06:34:59,2019-09-02 11:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 23, 34, 59), _2=datetime.datetime(2019, 9, 2, 1, 34, 59))\",moving-mall,dd40c,1\n"
    "7,2019-09-02 06:34:59,2019-09-02 11:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 23, 34, 59), _2=datetime.datetime(2019, 9, 2, 1, 34, 59))\",moving-mall,dd40c,1\n"
    "8,2019-09-02 06:34:59,2019-09-02 11:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 23, 34, 59), _2=datetime.datetime(2019, 9, 2, 1, 34, 59))\",moving-mall,dd40c,1\n"
    "9,2019-09-02 06:34:59,2019-09-02 11:34:59,\"Row(_1=datetime.datetime(2019, 9, 1, 23, 34, 59), _2=datetime.datetime(2019, 9, 2, 1, 34, 59))\",moving-mall,dd40c,1\n"
    "10,2019-09-02 12:34:59,2019-09-02 17:34:59,\"Row(_1=datetime.datetime(2019, 9, 2, 5, 34, 59), _2=datetime.datetime(2019, 9, 2, 7, 34, 59))\",moving-mall,dd40c,1\n"
    "11,2019-09-02 12:34:59,2019-09-02 17:34:59,\"Row(_1=datetime.datetime(2019, 9, 2, 5, 34, 59), _2=datetime.datetime(2019, 9, 2, 7, 34, 59))\",moving-mall,dd40c,1\n"
    "12,2019-09-02 12:34:59,2019-09-02 17:34:59,\"Row(_1=datetime.datetime(2019, 9, 2, 5, 34, 59), _2=datetime.datetime(2019, 9, 2, 7, 34, 59))\",moving-mall,dd40c,1";

    // create db object with new table
    DataBase db (dbname);

    // correctly format csv into a 2d vector that can be added to the database/table
    auto rows = db.processCSV(from_socket);
    // add to table
    db.insertRows(rows);

    // it is assumed that we know the timestamp and userID
    // you will have to format the timestamp as YYYYMMDDHHMMSS
    // to get the values within your stream.
    std::string timestamp = "20190901113459";
    std::string userID = "dd40c";
    // this gets the semantic location attributes
    std::vector<std::basic_string<char>> locations = db.getSemanticLocations(timestamp, userID);
    // this prints them
    for (auto &location: locations) {
        std::cout << location << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter

} // tests
} // mguard
