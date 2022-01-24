#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <chrono>
#include <unistd.h>
#include <iostream>

class Controller
{
public:
  Controller (const ndn::Name& aaPrefix);

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