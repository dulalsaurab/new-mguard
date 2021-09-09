#include <mguard/server/data-adapter.hpp>
#include "tests/boost-test.hpp"
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/dummy-client-face.hpp>

namespace mguard {

BOOST_AUTO_TEST_SUITE(TestDataAdapter)

BOOST_AUTO_TEST_CASE(Constructor)
{
  ndn::util::DummyClientFace c1;
  // ndn::security::KeyChain keychain;
  std::vector<std::string> streams;
  // ndn::security::Certificate producerCert, authorityCert;
//  try
//  {
//    producerCert = keychain.getPib().getIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate();
//    authorityCert = keychain.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate();
//  }
//  catch(const std::exception& e)
//  {
//    std::cerr << e.what() << '\n';
//  }
  // std::cout << producerCert.getMetaInfo() << std::endl;
  // streams.push_back("org.md2k/mguard/dd40c/gps/phone.csv");
  DataAdapter da("/mguard/aa", "/mguard/producer");
  // da.readData(streams);
}

BOOST_AUTO_TEST_SUITE_END()

} // mguard