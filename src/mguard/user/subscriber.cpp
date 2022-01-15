#include "subscriber.hpp"
#include <string>
#include <iostream>
#include <ndn-cxx/util/logger.hpp>

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.subscriber);

namespace mguard {
namespace subscriber {

Subscriber::Subscriber(ndn::Face& face, const ndn::Name& syncPrefix, 
                   const ndn::Name& userPrefix, ndn::time::milliseconds syncInterestLifetime,
                   const SyncUpdateCallback& syncUpdateCallback)
: m_syncUpdateCallback(syncUpdateCallback)
{
  NDN_LOG_DEBUG("Using PSync");
//   m_syncLogic = std::make_shared<psync::FullConsumer>(80,
//                    face, syncPrefix, userPrefix,
//                    std::bind(&Subscriber::onSyncUpdate, this, _1),
//                    syncInterestLifetime);
}

void
Subscriber::onSyncUpdate(const std::vector<psync::MissingDataInfo>& updates)
{}

} // producer
} // mguard