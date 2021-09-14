#include <mguard/server/data-adapter.hpp>
#include <mguard/server/util/stream.hpp>

#include <mguard/server/common.hpp>
#include "tests/boost-test.hpp"
// #include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

namespace mguard {

BOOST_AUTO_TEST_SUITE(TestDataAdapter)

BOOST_AUTO_TEST_CASE(Constructor)
{
  std::vector<std::string> attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  auto dataPath = DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";
  util::Stream stream("/org.md2k/mguard/dd40c/gps/phone", attributes, dataPath);

  ndn::util::DummyClientFace face;
  ndn::security::KeyChain k1;
  // std::vector<std::string> streams;
  // streams.push_back("/org.md2k/mguard/dd40c/gps/phone.csv");
  DataAdapter da(face, k1, "/mguard/aa", "/mguard/producer");
  da.readData(stream);
}

BOOST_AUTO_TEST_SUITE_END()

} // mguard