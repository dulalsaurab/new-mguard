#include <mguard/server/data-adapter.hpp>
#include <mguard/server/util/stream.hpp>

#include <mguard/server/common.hpp>

#include "tests/boost-test.hpp"

#include "tests/io-fixture.hpp"

#include "tests/clock-fixture.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <chrono>
#include <unistd.h>

using namespace ndn;

namespace mguard {


BOOST_AUTO_TEST_SUITE(TestDataAdapter)

BOOST_AUTO_TEST_CASE(Constructor)
{
  std::vector<std::string> attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  auto dataPath = DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";
  util::Stream stream("/org.md2k/mguard/dd40c/gps/phone", attributes, dataPath);

  ndn::security::KeyChain k1;
  boost::asio::io_service ioService;
  ndn::Face face;

  auto aaCert = k1.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate();
  auto pCert = k1.getPib().getIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate();
  
  // init data adapter, this will also init abe producer
  DataAdapter da(face, k1, "/mguard/producer", pCert, aaCert);

  ndn::Scheduler m_scheduler(face.getIoService());

  // sleep for 20ms to make sure abe-producer is init properly
  m_scheduler.schedule(20_ms, [&] {
    da.readData(stream);
  });

  da.run();


}

BOOST_AUTO_TEST_SUITE_END()

} // mguard