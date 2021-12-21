#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

// #include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <chrono>
#include <unistd.h>
#include <iostream>

using namespace ndn;

class ABEHelper
{
public:
  ABEHelper(const ndn::Name& aaPrefix)
  : m_keyChain("pib-sqlite3:/Users/sdulal/Documents/PROJECTS/mguard/certs/.ndn", "tpm-file")
  , m_aaCert(m_keyChain.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate())
  , m_attrAuthority(m_aaCert, m_face, m_keyChain)
  {
    std::cout << m_aaCert;
    std::cout << m_keyChain.getPib().getIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate();
  }
  
  ndn::nacabe::KpAttributeAuthority&
  getAttrAuthority()
  {
    return m_attrAuthority;
  }

  void
  run()
  {
    m_face.processEvents();
  }

private:
  ndn::Face m_face;
  ndn::security::KeyChain m_keyChain;
  ndn::Name aaPrefix;
  ndn::security::Certificate m_aaCert;
  ndn::nacabe::KpAttributeAuthority m_attrAuthority;
};


int main ()
{
  // boost::filesystem::path dir;
  // dir = "/Users/sdulal/Documents/PROJECTS/mguard/certs/";
  // ::setenv("HOME", dir.c_str(), 1);
  
  // DummyClientFace aaFace(io, m_keyChain, {true, true});

  // // // ndn::security::KeyChain keyChain
  // auto identity = m_keyChain.createIdentity(identityName, params);
  // m_identities.insert(identityName);
  // return identity;

  // auto aaIdentity = addIdentity("/mguard/aa");
  // auto aaCert = aaIdentity.getDefaultKey().getDefaultCertificate();

  ABEHelper abe("/mguard/aa");
  abe.run();
}