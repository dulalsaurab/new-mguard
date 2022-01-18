#ifndef MGUARD_PUBLISHER_HPP
#define MGUARD_PUBLISHER_HPP

// #include "pre-processor.hpp"
#include "file-processor.hpp"
#include "util/stream.hpp"

#include <ndn-cxx/face.hpp>
#include <PSync/partial-producer.hpp>

#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <unordered_map>

namespace mguard {

/*
if use_manifest is set to false, manifest will not be used, application data will be publised directly.
*/
const bool USE_MANIFEST = true;

// manifest will be published after receiving 100 data units
const int MANIFEST_BATCH_SIZE = 100;

// if next update is not received withing 100 ms, the manifest will be publised, this can override batch size
const int MAX_UPDATE_WAIT_TIME = 100;

namespace tlv {

enum
{
  mGuardContent = 128,
  DataRow = 129
};
} // namespace tlv


class Publisher
{

public:
  Publisher(ndn::Face& face, ndn::security::KeyChain& keyChain,
              const ndn::Name& producerPrefix,
              const ndn::security::Certificate& producerCert,
              const ndn::security::Certificate& attrAuthorityCertificate);

  
  void
  doUpdate(ndn::Name& manifestName);

  void
  publish(ndn::Name dataName, std::string data, util::Stream& stream);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block) const;

  const ndn::Block&
  wireEncode();

  void
  setInterestFilter(const ndn::Name& name, const bool loopback = false);

  void
  processInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  onRegistrationSuccess(const ndn::Name& name);

  void
  onRegistrationFailed(const ndn::Name& name);
  
  void
  sendApplicationNack(const ndn::Name& name);

private:
  ndn::Face& m_face;
  ndn::security::KeyChain& m_keyChain;
  ndn::Scheduler m_scheduler;
  mutable ndn::Block m_wire;
  psync::PartialProducer m_partialProducer;
  
  // FileProcessor m_fileProcessor;
  std::string m_tempRow;
  ndn::Name m_attrAuthorityPrefix;
  ndn::Name m_producerPrefix;
  ndn::security::Certificate m_producerCert;
  ndn::security::Certificate m_authorityCert;
  ndn::nacabe::CacheProducer m_abe_producer;
  std::unordered_map<ndn::Name, std::shared_ptr<ndn::Data>> m_dataBuffer; //need to limit the size of the buffer


};

} // mguard

#endif // MGUARD_PUBLISHER_HPP