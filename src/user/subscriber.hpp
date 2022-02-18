#ifndef MGUARD_SUBSCRIBER_HPP
#define MGUARD_SUBSCRIBER_HPP

#include <PSync/consumer.hpp>
#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/consumer.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <string>
#include <chrono>
#include <thread>

using namespace ndn::time_literals;

namespace mguard {

struct SyncDataInfo
{
  ndn::Name prefix;
  uint64_t highSeq;
  uint64_t lowSeq;
};

typedef std::function<void(const std::vector<SyncDataInfo>& updates)> SyncUpdateCallback;

namespace subscriber {
namespace tlv {
}

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class Subscriber
{
public:
  Subscriber(const ndn::Name& consumerPrefix,
             const ndn::Name& syncPrefix, 
             ndn::time::milliseconds syncInterestLifetime,
             std::vector<std::string>& subscriptionList,
             const SyncUpdateCallback& syncUpdateCallback);

  void
  run();

  void
  stop();

  void
  expressInterest(const ndn::Name& name, bool canBePrefix = false, bool mustBeFresh = false);

  void
  onData(const ndn::Interest& interest, const ndn::Data& data);

  void
  onTimeout(const ndn::Interest& interest);

  void
  subscribe(ndn::Name streamName);
  
  void
  unsubscribe(const ndn::Name& topic);

  void
  receivedHelloData(const std::map<ndn::Name, uint64_t>& availStreams);

  void
  receivedSyncUpdates(const std::vector<psync::MissingDataInfo>& updates);

  void
  sendInterest();
  
  void
  wireDecode(const ndn::Block& wire);

  // abe callbacks
  void
  abeOnData(const ndn::Buffer& buffer);
  
  void 
  abeOnError(const std::string& errorMessage);

private:
  ndn::Face m_face;
  ndn::security::KeyChain m_keyChain;

  ndn::Name m_consumerPrefix;
  ndn::Name m_syncPrefix;
  std::vector<std::string>& m_subscriptionList;
  // available streams are the ones received from psync
  // and eligible streams are determined from the policy
  std::unordered_map<ndn::Name, uint64_t> m_availableStreams;
  std::unordered_set<ndn::Name> m_eligibleStreams;
  ndn::nacabe::algo::PrivateKey decryptionKey;
  ndn::nacabe::Consumer m_abe_consumer;

  psync::Consumer m_psync_consumer;
  SyncUpdateCallback m_syncUpdateCallback;
};

} //namespace subscriber
} //namespace mguard

#endif // MGUARD_SUBSCRIBER_HPP