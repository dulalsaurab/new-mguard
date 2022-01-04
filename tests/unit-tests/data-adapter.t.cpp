#include <mguard/server/data-adapter.hpp>
#include <mguard/server/util/stream.hpp>

#include <mguard/server/common.hpp>

#include "test-common.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <chrono>
#include <unistd.h>
#include <thread>

using namespace ndn;
using DummyClientFace = ndn::util::DummyClientFace;

namespace mguard {
namespace tests{

class DataAdapterFixture: IdentityTimeFixture
{
// public:
//   DataAdapterFixture()
//   :

};

BOOST_FIXTURE_TEST_SUITE(TestDataAdapter, IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
  std::vector<std::string> attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  auto dataPath = DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";
  util::Stream stream("/org.md2k/mguard/dd40c/gps/phone", attributes, dataPath);
  
  // DummyClientFace face(io, m_keyChain, {true, true});
  // DummyClientFace aaFace(io, m_keyChain, {true, true});
  // face.linkTo(aaFace);
  // ndn::security::KeyChain m_keyChain;
  // ndn::Face m_face;
  // ndn::security::KeyChain m_keyChain;
  // auto aaIdentity = addIdentity("/mguard/aa");
  // auto pIdentity = addIdentity("/mguard/producer");
  // auto aaCert = aaIdentity.getDefaultKey().getDefaultCertificate();
  // auto pCert = pIdentity.getDefaultKey().getDefaultCertificate();
  // boost::filesystem::path dir;
  // dir = "/Users/sdulal/Documents/PROJECTS/mguard/certs/";
  // ::setenv("HOME", dir.c_str(), 1);
  // auto aaIdentity = addIdentity("/mguard/aa");
  // auto pIdentity = addIdentity("/mguard/producer");
  // auto aaCert = aaIdentity.getDefaultKey().getDefaultCertificate();
  // auto pCert = pIdentity.getDefaultKey().getDefaultCertificate();
  // ndn::security::KeyChain m_keyChain("pib-sqlite3:/Users/sdulal/Documents/PROJECTS/mguard/certs/.ndn", "tpm-file");
  // ndn::security::Keychain m_keyChain("pib-sqlite3:/Users/sdulal/Documents/PROJECTS/mguard/certs/.ndn", "tpm-file")
  // auto abc = m_keyChain.createIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate();
  // auto abc = m_keyChain.getPib().getIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate();
  // std::cout << abc;

  // // init data adapter, this will also init abe producer
  mguard::DataAdapter da("/mguard/producer", "/mguard/aa");
  // ndn::nacabe::KpAttributeAuthority aa(aaCert, aaFace, m_keyChain);
  da.run();
  // advanceClocks(time::milliseconds(20), 5);
  da.publishDataUnit(stream);
  // da.run();

}

BOOST_AUTO_TEST_CASE(publishData)
{
  // std::vector<std::string> attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  // auto dataPath = DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";
  // util::Stream stream("/org.md2k/mguard/dd40c/gps/phone", attributes, dataPath);
  // da.publishDataUnit(stream);
  // // ndn::Face m_face;
  // ndn::security::KeyChain m_keyChain;
  // std::cout << m_keyChain.getPib().getIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate();
  // DataAdapter da("/mguard/producer", "/mguard/aa");
}


BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter

} // tests
} // mguard
