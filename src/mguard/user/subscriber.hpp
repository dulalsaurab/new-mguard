

#ifndef MGUARD_SUBSCRIBER_HPP
#define MGUARD_SUBSCRIBER_HPP

#include <PSync/full-producer.hpp>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/time.hpp>

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
  Subscriber(ndn::Face& face, const ndn::Name& syncPrefix,
           const ndn::Name& userPrefix, ndn::time::milliseconds syncInterestLifetime,
           const SyncUpdateCallback& syncUpdateCallback);

  void
  setInterestFilter(const ndn::Name& name, const bool loopback);

  void
  processInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  sendData(const ndn::Name& name);

  void
  sendInterest();
  
  void
  onSyncUpdate(const std::vector<psync::MissingDataInfo>& updates);

  void
  subscribe(const ndn::Name& topic);

  void
  unsubscribe(const ndn::Name& topic);

  void
  changePolicy();

private:
  // std::shared_ptr<psync::PartialConsumer> m_partialConsumer;
  ndn::Face m_face;
  SyncUpdateCallback m_syncUpdateCallback;


};

} //namespace subscriber
} //namespace mguard

#endif // MGUARD_SUBSCRIBER_HPP