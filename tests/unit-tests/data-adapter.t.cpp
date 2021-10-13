#include <mguard/server/data-adapter.hpp>
#include <mguard/server/util/stream.hpp>

#include <mguard/server/common.hpp>

#include "test-common.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <chrono>
#include <unistd.h>

using namespace ndn;
using DummyClientFace = ndn::util::DummyClientFace;

namespace mguard {
namespace tests{

BOOST_FIXTURE_TEST_SUITE(TestDataAdapter, IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
  std::vector<std::string> attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  auto dataPath = DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";
  util::Stream stream("/org.md2k/mguard/dd40c/gps/phone", attributes, dataPath);

  DummyClientFace face(io, m_keyChain, {true, true});
  DummyClientFace aaFace(io, m_keyChain, {true, true});
  face.linkTo(aaFace);

  auto aaIdentity = addIdentity("/mguard/aa");
  auto pIdentity = addIdentity("/mguard/producer");
  auto aaCert = aaIdentity.getDefaultKey().getDefaultCertificate();
  auto pCert = pIdentity.getDefaultKey().getDefaultCertificate();
  
  // init data adapter, this will also init abe producer
  DataAdapter da(face, m_keyChain, "/mguard/producer", pCert, aaCert);
  ndn::nacabe::KpAttributeAuthority aa(aaCert, aaFace, m_keyChain);

  advanceClocks(time::milliseconds(20), 5);
  da.readData(stream);

}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter

} // tests
} // mguard
