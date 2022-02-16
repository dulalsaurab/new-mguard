#ifndef MGUARD_CONTROLLER_HPP
#define MGUARD_CONTROLLER_HPP

#include "parser.hpp"

#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <chrono>
#include <unistd.h>
#include <iostream>

namespace mguard {
namespace controller {

class Controller
{
public:
  Controller(const ndn::Name& controllerPrefix, const ndn::Name& aaPrefix, 
             const std::string& availableStreamsFilePath);

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

  void
  processPolicy(std::string policyPath);

  void
  setInterestFilter(const ndn::Name& name, const bool loopback = false);

  void
  processInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  onRegistrationSuccess(const ndn::Name& name);

  void
  onRegistrationFailed(const ndn::Name& name);

  void
  sendData(const ndn::Name& name);

  void
  sendApplicationNack(const ndn::Name& name);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block);

  const ndn::Block&
  wireEncode();

private:
  
  struct policyDetails
  {
    std::string policyId;
    std::list <ndn::Name> streams;
    std::string abePolicy;
    ndn::nacabe::algo::PrivateKey decryptionKey;
  };

  // ndn::Name --> requesterID or name or prefix, probably this will be cert 
  std::map<ndn::Name, policyDetails> m_policyMap;
  policyDetails m_temp_policyDetail; // we use this for encoding requested key/streams

  ndn::Face m_face;
  ndn::security::KeyChain m_keyChain;
  mutable ndn::Block m_wire;
  
  const ndn::Name& m_controllerPrefix;
  const ndn::Name& m_aaPrefix;

  parser::PolicyParser m_policyParser;
  ndn::security::Certificate m_aaCert;
  ndn::nacabe::KpAttributeAuthority m_attrAuthority;

};

} // namespace controller
} // namespace mguard

#endif // MGUARD_CONTROLLER_HPP