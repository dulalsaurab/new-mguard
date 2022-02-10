#include "subscriber.hpp"

#include <iostream>

NDN_LOG_INIT(mguard.subscriber);

namespace mguard {
namespace subscriber {

Subscriber::Subscriber(const ndn::Name& syncPrefix, 
                       ndn::time::milliseconds syncInterestLifetime,
                       std::vector<std::string>& subscriptionList,
                       const SyncUpdateCallback& syncUpdateCallback)
: m_syncPrefix(syncPrefix)
, m_subscriptionList(subscriptionList)
, m_consumer(m_syncPrefix, m_face,
             std::bind(&Subscriber::receivedHelloData, this, _1),
             std::bind(&Subscriber::receivedSyncUpdates, this, _1),
             2, 0.001) // 2 = expected number of prefix to subscriber to, need to handle this differently later
, m_syncUpdateCallback(syncUpdateCallback)
{
  NDN_LOG_DEBUG("Subscriber initialized");
  
  // This starts the consumer side by sending a hello interest to the producer
  // When the producer responds with hello data, receivedHelloData is called
  m_consumer.sendHelloInterest();

  // m_eligibleStreams.insert("/org.md2k/mguard/dd40c/gps/phone/manifest");
  /* TODO: 
    1. fetch consumer's decryption key, and the eligible streams from controller
    2. store the key and the streams
  */
}

void
Subscriber::run()
{
  try {
    NDN_LOG_INFO("Starting Face");
    m_face.processEvents();
  }
  catch (const std::exception& ex)
  {
    NDN_LOG_ERROR("Face error: " << ex.what()); 
    NDN_THROW(Error(ex.what()));
  }
}

void
Subscriber::stop()
{
  NDN_LOG_DEBUG("Shutting down face: ");
  m_face.shutdown();
}

void
Subscriber::subscribe(ndn::Name streamName)
{
  // convert the streamName into manifest, because that's what is published by the sync
  streamName.append("manifest");
  auto it = m_availableStreams.find(streamName);
  if (it == m_availableStreams.end()) {
    NDN_LOG_INFO("Stream" << streamName << "not available for subscription");
    return;
  }
  NDN_LOG_INFO("Subscribing to: " << streamName);
  m_consumer.addSubscription(streamName, it->second);
  m_consumer.sendSyncInterest();
}

void
Subscriber::receivedHelloData(const std::map<ndn::Name, uint64_t>& availStreams)
{
  // store all the streams names and their latest seq number
  for (const auto& it: availStreams) {
    NDN_LOG_DEBUG (" stream name: " << it.first << " latest seqNum" << it.second);
    m_availableStreams[it.first] = it.second;
  }
  
  // subscribe to streams present in the subscription list 
  for (auto stream : m_subscriptionList)
  {
    subscribe(stream);
  }
}

void
Subscriber::receivedSyncUpdates(const std::vector<psync::MissingDataInfo>& updates)
{
  for (const auto& update : updates) {
    for (uint64_t i = update.lowSeq; i <= update.highSeq; i++) {
      NDN_LOG_INFO("Update: " << update.prefix << "/" << i);
    }
  }
}
} // subscriber
} // mguard