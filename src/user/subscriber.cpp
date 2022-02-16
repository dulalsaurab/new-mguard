#include "subscriber.hpp"
#include "common.hpp"

#include <nac-abe/attribute-authority.hpp>

#include <iostream>

NDN_LOG_INIT(mguard.subscriber);

namespace mguard {
namespace subscriber {

Subscriber::Subscriber(const ndn::Name& consumerPrefix,
                       const ndn::Name& syncPrefix,
                       ndn::time::milliseconds syncInterestLifetime,
                       std::vector<std::string>& subscriptionList,
                       const SyncUpdateCallback& syncUpdateCallback)
: m_consumerPrefix(consumerPrefix)
, m_syncPrefix(syncPrefix)
, m_subscriptionList(subscriptionList)
, m_consumer(m_syncPrefix, m_face,
             std::bind(&Subscriber::receivedHelloData, this, _1),
             std::bind(&Subscriber::receivedSyncUpdates, this, _1),
             2, 0.001) // 2 = expected number of prefix to subscriber to, need to handle this differently later
, m_syncUpdateCallback(syncUpdateCallback)
{
  NDN_LOG_DEBUG("Subscriber initialized");
  
  // get policy details from controller
  try {
    ndn::Name interestName= "/mguard/controller";
    interestName.append(m_consumerPrefix);
    NDN_LOG_DEBUG("Getting policy detail data, send interest: " << interestName);
    expressInterest(interestName);
  }
  catch (const std::exception& e)
  {
    NDN_LOG_ERROR("error: " << e.what()); 
  }

  // This starts the consumer side by sending a hello interest to the producer
  // When the producer responds with hello data, receivedHelloData is called
  // m_consumer.sendHelloInterest();

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
Subscriber::expressInterest(const ndn::Name& name)
{
  NDN_LOG_INFO("Sending interest: "  << name);
  ndn::Interest interest(name);
  interest.setCanBePrefix(false);
  // interest.setMustBeFresh(true); //set true if want data explicit from producer.
  interest.setInterestLifetime(160_ms);

  m_face.expressInterest(interest,
                         bind(&Subscriber::onData, this, _1, _2),
                         bind(&Subscriber::onTimeout, this, _1),
                         bind(&Subscriber::onTimeout, this, _1));
}

void
Subscriber::onData(const ndn::Interest& interest, const ndn::Data& data)
{
  NDN_LOG_INFO("Data received for: " << interest.getName());

  // std::cout << data.getContent() << std::endl;
  wireDecode(data.getContent());

}
void
Subscriber::onTimeout(const ndn::Interest& interest)
{
  NDN_LOG_INFO("Interest: " << interest.getName() << " timed out ");
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

void
Subscriber::wireDecode(const ndn::Block& wire)
{
  m_eligibleStreams.clear();
  wire.parse();
  auto val = wire.elements_begin();
  if (val != wire.elements_end() && val->type() == mguard::tlv::mGuardController)
  {
    val->parse();
    for (auto it = val->elements_begin(); it != val->elements_end(); ++it) {
      if (it->type() == ndn::tlv::DescriptionKey)
      {
        std::cout << ndn::encoding::readString(*it) << std::endl;
      }
      else {
        m_eligibleStreams.emplace(*it); 
      }
    }
  }
}

} // subscriber
} // mguard