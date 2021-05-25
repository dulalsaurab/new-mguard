#include "publisher.hpp"
#include <string>
#include <iostream>
#include <ndn-cxx/util/logger.hpp>

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.publisher);

namespace mguard {
namespace Publisher {

Publisher::Publisher(ndn::Face& face, const ndn::Name& syncPrefix, 
                   const ndn::Name& userPrefix, ndn::time::milliseconds syncInterestLifetime,
                   const SyncUpdateCallback& syncUpdateCallback)
: m_syncUpdateCallback(syncUpdateCallback)
, m_face(face)

{
  m_syncLogic = std::make_shared<psync::FullProducer>(80,
                     face, syncPrefix, userPrefix,
                     std::bind(&Publisher::onSyncUpdate, this, _1),
                     syncInterestLifetime);
}

void
Publisher::addUserNode(const ndn::Name& userPrefix)
{
    m_syncLogic->addUserNode(userPrefix);
}

void
Publisher::publishUpdate(const ndn::Name& userPrefix)
{
  NDN_LOG_TRACE("Publish sync update for prefix: " << userPrefix);
  auto seq_p = m_syncLogic->getSeqNo(userPrefix);
  NDN_LOG_INFO("Publishing update for: " << userPrefix << "/" << seq_p.value()+1);
  m_syncLogic->publishName(userPrefix);

}

void
Publisher::onSyncUpdate(const std::vector<psync::MissingDataInfo>& updates)
{
  NDN_LOG_INFO("Received Sync update event");
  std::vector<SyncDataInfo> dinfo;

  for (const auto& update : updates) {
    SyncDataInfo di;
    di.prefix = update.prefix;
    di.highSeq = update.highSeq;
    di.lowSeq = update.lowSeq;
    dinfo.insert(dinfo.begin(), di);
  }
  // For debug: print all the received updates
  printSyncUPdate(dinfo);
  m_syncUpdateCallback(dinfo);
}

void 
Publisher::printSyncUPdate(const std::vector<SyncDataInfo> updates)
{
  for (auto item: updates)
    {
      for (auto seq = item.lowSeq; seq <= item.highSeq; seq++)
      {
        ndn::Name prefix = item.prefix;
        NDN_LOG_DEBUG("Sync update received for prefix: " << prefix << "/" << seq);
      }
    }
}

} // publisher
} // mguard