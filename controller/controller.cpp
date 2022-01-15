#include "controller.hpp"

using namespace ndn;

Controller::Controller(const ndn::Name& aaPrefix)
// TODO: need to fix this
: m_keyChain("pib-sqlite3:/Users/sdulal/.ndn/", "tpm-file:/Users/sdulal/.ndn/ndnsec-tpm-file")
, m_aaCert(m_keyChain.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate())
, m_attrAuthority(m_aaCert, m_face, m_keyChain)
{
  std::cout << "Authority cert" << m_aaCert << std::endl;
}

// int main ()
// {
//   Controller abe("/mguard/aa");
//   abe.run();
// }