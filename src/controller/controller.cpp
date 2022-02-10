#include "controller.hpp"

using namespace ndn;

Controller::Controller(const ndn::Name& aaPrefix)
// TODO: need to fix this
: m_keyChain()
, m_aaCert(m_keyChain.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate())
, m_attrAuthority(m_aaCert, m_face, m_keyChain)
{
  struct policyDetails
  {
      std::list <std::string> streams;
      std::list <std::string> requesters;
      std::string decryption_key;
  };

  std::cout << "Authority cert" << m_aaCert << std::endl;
  std::map<std::string, policyDetails> policyMap;

  mguard::PolicyParser pp("policies/available_streams");
  pp.inputPolicy("default.policy");
  auto x = pp.getPolicyInfo();
  for (const std::string& requester : x.requesters) {
      m_attrAuthority.addNewPolicy(requester, x.abePolicy);
      policyMap.insert(x.policyIdentifier, {x.streams, x.requesters, m_attrAuthority.getPrivateKey(requester)});
  }

}
