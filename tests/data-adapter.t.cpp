#include <mguard/server/data-adapter.hpp>
#include <mguard/server/util/stream.hpp>

#include <mguard/server/common.hpp>
#include "tests/boost-test.hpp"

#include "tests/io-fixture.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <chrono>
#include <unistd.h>

using namespace ndn;

namespace mguard {

BOOST_AUTO_TEST_SUITE(TestDataAdapter)

BOOST_FIXTURE_TEST_CASE(Constructor, tests::IoFixture)
{
  std::vector<std::string> attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  auto dataPath = DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";
  util::Stream stream("/org.md2k/mguard/dd40c/gps/phone", attributes, dataPath);

  ndn::security::KeyChain k1;
  boost::asio::io_service ioService;

  ndn::Face face;
  // face(ioService, k1, ndn::util::DummyClientFace::Options{true, true});
  
  auto prefixId = face.registerPrefix("/mmGuard/aa",
    [&] (const Name& name) {
      std::cout << "reached here" << std::endl;
    },
    [&] (const Name& name, const std::string& ab) {
      std::cout << "reached here" << std::endl;
      std::cout << ab << std::endl;
    });
  
  // std::vector<std::string> streams;
  // streams.push_back("/org.md2k/mguard/dd40c/gps/phone.csv");
  std::cout << "processing stream" << std::endl;
  DataAdapter da(face, k1, "/mguard/aa", "/mguard/producer");
  da.run();
  // usleep(2000);
  // da.readData(stream);

  // ndn::Interest i1("/org.md2k/mguard/dd40c/gps/phone/DATA/40%3A59");

  // size_t nData = 0;
  // std::cout << "reached here" << std::endl;
  // face.expressInterest(i1,
  //                      [&] (const auto&, const auto&) { ++nData; },
  //                      std::bind([] { BOOST_FAIL("Unexpected Nack"); }),
  //                      std::bind([] { BOOST_FAIL("Unexpected timeout"); }));

  // da.processInterest(i1.getName(), i1);

  // advanceClocks(10_ms);
  // std::cout << "data count" << nData << std::endl;


}

BOOST_AUTO_TEST_SUITE_END()

} // mguard