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
  // TODO: need to fix this
  : m_keyChain("pib-sqlite3:/Users/sdulal/.ndn/", "tpm-file:/Users/sdulal/.ndn/ndnsec-tpm-file")
  , m_aaCert(m_keyChain.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate())
  , m_attrAuthority(m_aaCert, m_face, m_keyChain)
  {
    std::cout << "Authority cert" << m_aaCert << std::endl;
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
  ABEHelper abe("/mguard/aa");
  abe.run();
}