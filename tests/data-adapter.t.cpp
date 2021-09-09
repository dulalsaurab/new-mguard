#include <mguard/server/data-adapter.hpp>
#include "tests/boost-test.hpp"
#include <ndn-cxx/name.hpp>

namespace mguard {

BOOST_AUTO_TEST_SUITE(TestDataAdapter)

BOOST_AUTO_TEST_CASE(Constructor)
{
  std::vector<std::string> streams;
  streams.push_back("org.md2k/mguard/dd40c/gps/phone.csv");
  // DataAdapter da;
  // da.readData(streams);
}

BOOST_AUTO_TEST_SUITE_END()

} // mguard