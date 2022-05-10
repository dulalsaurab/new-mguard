#include "publisher.hpp"
#include "common.hpp"

NDN_LOG_INIT(mguard.Publisher);

namespace mguard {

Publisher::Publisher(ndn::Face& face, ndn::security::KeyChain& keyChain,
                    const ndn::Name& producerPrefix,
                    const ndn::security::Certificate& producerCert,
                    const ndn::security::Certificate& attrAuthorityCertificate)
: m_face(face)
, m_keyChain(keyChain)
, m_scheduler(m_face.getIoService())

// 40 = expected number of entries also will be used as IBF size
// syncPrefix = /org.md2k/sync, userPrefix = /org.md2k/uprefix <--- this will be changed
, m_partialProducer(40, m_face, "/ndn/org/md2k", "/ndn/org/md2k/uprefix")

, m_producerPrefix(producerPrefix)
, m_producerCert(producerCert)
, m_authorityCert(attrAuthorityCertificate)
, m_abe_producer(m_face, m_keyChain, m_producerCert, m_authorityCert)
{
  // sleep to init kp-abe producer
  std::this_thread::sleep_for (std::chrono::seconds(1));
}

void
Publisher::doUpdate(ndn::Name& manifestName)
{
  m_partialProducer.publishName(manifestName);
  uint64_t seqNo =  m_partialProducer.getSeqNo(manifestName).value();
  NDN_LOG_DEBUG("Publish sync update for the name/manifest: " << manifestName << " sequence Number: " << seqNo);
}

void
Publisher::scheduledManifestForPublication(util::Stream& stream)
{
  auto name = stream.getName();
  auto itr = m_scheduledIds.find(name);
  NDN_LOG_DEBUG("Scheduling manifest: " << name << " for publication");
  
  if (itr != m_scheduledIds.end()) {
    NDN_LOG_DEBUG("Manifest: " << name << " was already scheduled, updating the schedule");
    itr->second.cancel();
    auto scheduleId = m_scheduler.schedule(MAX_UPDATE_WAIT_TIME, [&] {
                      publishManifest(stream);
                      doUpdate(stream.getManifestName());
                      NDN_LOG_DEBUG("Updated manifest: " << stream.getName() << " via scheduling");
                    });
    itr->second = scheduleId;
  }
  else {
    auto scheduleId = m_scheduler.schedule(MAX_UPDATE_WAIT_TIME, [&] {
                      publishManifest(stream);
                      doUpdate(stream.getManifestName());
                      NDN_LOG_DEBUG("Updated manifest: " << stream.getName() << " via scheduling");
                    });
    m_scheduledIds.emplace(name, scheduleId);
  }
}

void
Publisher::publish(ndn::Name& dataName, std::string data, util::Stream& stream)
{
    // create a manifest, and append each <data-name>/<implicit-digetst> to the manifest
    // Manifest name: <stream name>/manifest/<seq-num>
    NDN_LOG_DEBUG("Publishing data: " << data);

    std::shared_ptr<ndn::Data> enc_data, ckData;
    try {
        NDN_LOG_DEBUG("Encrypting data: " << dataName);
        auto dataSufix = dataName.getSubName(2);
        NDN_LOG_TRACE("--------- data suffix: " << dataSufix);

        // debugging
        for (auto& a: stream.getAttributes())
          NDN_LOG_DEBUG("attribute: " << a);

        std::tie(enc_data, ckData) = m_abe_producer.produce(dataSufix, stream.getAttributes(), 
                                                            {reinterpret_cast<const uint8_t *>(data.c_str()), data.size()});
    }
    catch(const std::exception& e) {
      NDN_LOG_ERROR("Encryption failled");
      std::cerr << e.what() << '\n';
      // return false;
    }
    //  encrypted data is created, store it in the buffer and publish it
    NDN_LOG_INFO("data: " << enc_data->getFullName() << " ckData: " << ckData->getFullName());

    // store the data into the repo, the insertion uses tcp bulk insertion protocol
    try {
      if ((m_repoInserter.writeDataToRepo(*enc_data)) && (m_repoInserter.writeDataToRepo(*ckData)))
        NDN_LOG_DEBUG("data and cKdata insertion completed"); 
    }
    catch(const std::exception& e) {
      NDN_LOG_ERROR("data and cKdata insertion failed");
      std::cerr << e.what() << '\n';
    }

    bool doPublishManifest = stream.updateManifestList(enc_data->getFullName());

    // manifest are publihsed to sync after receiving X (e.g. 10) number of application data or if
    // "t" time has passed after receiving the last application data.
    if(doPublishManifest) {
      cancleIfManifestScheduledForPublication(stream.getName());
      // create manifest data packet, and insert it into the repo
      publishManifest(stream);
      doUpdate(stream.getManifestName());
    }
    else
      scheduledManifestForPublication(stream);
}

void
Publisher::publishManifest(util::Stream& stream)
{
  auto dataName = stream.getManifestName();
  m_partialProducer.addUserNode(dataName);

  auto prevSeqNum = m_partialProducer.getSeqNo(stream.getManifestName()).value();
  dataName.appendNumber(prevSeqNum + 1);
  auto manifestData = std::make_shared<ndn::Data>(dataName);
  
  m_temp = stream.getManifestList();

  manifestData->setContent(wireEncode());

  NDN_LOG_DEBUG ("Manifest seqNumber: " << prevSeqNum + 1);
  
  m_keyChain.sign(*manifestData);

  try {
    if ((m_repoInserter.writeDataToRepo(*manifestData)))
      NDN_LOG_DEBUG("Successfully inserted manifest into the repo");
      m_temp.clear(); // clear temp variable
      stream.resetManifestList(); // clear manifest list
      m_wire.reset(); // reset the wire for new content
  }
  catch(const std::exception& e) {
    NDN_LOG_ERROR("Failed to insert mainfest into the repo");
    std::cerr << e.what() << '\n';
  }
}

const ndn::Block&
Publisher::wireEncode() const
{
  if (m_wire.hasWire()) {
    return m_wire;
  }

  ndn::EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  ndn::EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

template <ndn::encoding::Tag TAG>
size_t
Publisher::wireEncode(ndn::EncodingImpl<TAG> &encoder) const
{
  size_t totalLength = 0;
  
  for (auto it = m_temp.rbegin(); it != m_temp.rend(); ++it) {
    NDN_LOG_DEBUG ("Encoding data name: " << *it);
    totalLength += it->wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(mguard::tlv::mGuardPublisher);
  
  return totalLength;
}

} //mguard