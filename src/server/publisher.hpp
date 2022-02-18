#ifndef MGUARD_PUBLISHER_HPP
#define MGUARD_PUBLISHER_HPP

#include "file-processor.hpp"
#include "util/stream.hpp"
#include "util/repo-inserter.cpp"

#include <PSync/partial-producer.hpp>
#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/logger.hpp>

#include <unordered_map>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

namespace mguard {

/*
if use_manifest is set to false, manifest will not be used, application data will be publised directly.
*/
const bool USE_MANIFEST = true;

// manifest will be published after receiving 100 data units
const int MANIFEST_BATCH_SIZE = 100;

// if next update is not received withing 100 ms, the manifest will be publised, this can override batch size
const int MAX_UPDATE_WAIT_TIME = 100;

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

  void
  publishManifest(util::Stream& stream);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block) const;

  const ndn::Block&
  wireEncode() const;
  
private:
  ndn::Face& m_face;
  ndn::security::KeyChain& m_keyChain;
  ndn::Scheduler m_scheduler;
  mutable ndn::Block m_wire;
  psync::PartialProducer m_partialProducer;
  util::RepoInserter m_repoInserter;

  // FileProcessor m_fileProcessor;
  std::vector<ndn::Name> m_temp;
  ndn::Name m_attrAuthorityPrefix;
  ndn::Name m_producerPrefix;
  ndn::security::Certificate m_producerCert;
  ndn::security::Certificate m_authorityCert;
  ndn::nacabe::CacheProducer m_abe_producer;
};

} // mguard

#endif // MGUARD_PUBLISHER_HPP