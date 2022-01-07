#include "common.hpp"
#include "publisher.hpp"
#include "util/stream.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/encoding/block-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/sha256.hpp>

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

NDN_LOG_INIT(mguard.Publisher);

namespace mguard
{

Publisher::Publisher(ndn::Face& face, ndn::security::KeyChain& keyChain,
                    const ndn::Name& producerPrefix,
                    const ndn::security::Certificate& producerCert,
                    const ndn::security::Certificate& attrAuthorityCertificate)
: m_face(face)
, m_keyChain(keyChain)
, m_scheduler(m_face.getIoService())

// 40 = expected number of entries also will be used as IBF size
// syncPrefix = /org.md2k/sync, userPrefix = /org.md2k/uprefix <--- this will be changed
, m_partialProducer(40, m_face, m_keyChain, m_scheduler, "/org.md2k", "/org.md2k/uprefix")

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
Publisher::publish(ndn::Name dataName, std::string data, util::Stream& stream)
{
    // TODO: create a manifest, and append each <data-name>/<implicit-digetst> to the manifest
    // Manifest name: <stream name>/manifest/<seq-num>
    NDN_LOG_DEBUG("Publishing data: " << data);

    std::shared_ptr<ndn::Data> enc_data, ckData;
    try
    {
        NDN_LOG_DEBUG("Encrypting data: " << dataName);
        unsigned char* byteptr = reinterpret_cast<unsigned char *>(&data);
        std::tie(ckData, enc_data) = m_abe_producer.produce(dataName, stream.getAttributes(), byteptr, sizeof(byteptr));
        // m_keyChain.sign(*enc_data, ndn::signingWithSha256());
    }
    catch(const std::exception& e)
    {
        NDN_LOG_ERROR("Encryption failled");
        std::cerr << e.what() << '\n';
        // return false;
    }
    //  encrypted data is created, store it in the buffer and publish it
    NDN_LOG_INFO("data: " << enc_data->getFullName() << " ckData: " << ckData->getFullName());
    // m_dataBuffer.emplace(dataName, enc_data); // we dont need data buffer : erase this
    
    bool publishManifest = stream.updateManifestList(enc_data->getFullName());

    if(publishManifest) {
      // use partial sync to publish data;
      doUpdate(stream.getManifestName());
    }
}

const ndn::Block &
Publisher::wireEncode()
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
  totalLength += prependStringBlock(encoder, tlv::DataRow, m_tempRow);
  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::mGuardContent);

  return totalLength;
}

void
Publisher::setInterestFilter(const ndn::Name& name, const bool loopback)
{
  NDN_LOG_INFO("Setting interest filter on: " << name);
  m_face.setInterestFilter(ndn::InterestFilter(name).allowLoopback(false),
                           std::bind(&Publisher::processInterest, this, _1, _2),
                           std::bind(&Publisher::onRegistrationSuccess, this, _1),
                           std::bind(&Publisher::onRegistrationFailed, this, _1));
}


void
Publisher::processInterest(const ndn::Name& name, const ndn::Interest& interest)
{
  // need to encapsulate this data into mguard data packet
  auto it = m_dataBuffer.find(name);
  if (it != m_dataBuffer.end()) {
    auto data = it->second;
    NDN_LOG_INFO("Sending data for name: " << name << "data" << *data);
    ndn::Data _data (*it->second);
    m_keyChain.sign(_data);
    m_face.put(_data);
    // once the data is schedule the corresponding entry deletion from the buffer
    m_dataBuffer.erase(it);
  }
  else {
    // data is not available in the buffer, send application nack
    sendApplicationNack(name);
  }
}

void
Publisher::sendApplicationNack(const ndn::Name& name)
{
  NDN_LOG_INFO("Sending application nack");
  ndn::Name dataName(name);
  ndn::Data data(dataName);
  data.setContentType(ndn::tlv::ContentType_Nack);
  m_keyChain.sign(data);
  m_face.put(data);
}


void
Publisher::onRegistrationSuccess(const ndn::Name& name)
{
  NDN_LOG_INFO("Successfully registered prefix: " << name);
}

void
Publisher::onRegistrationFailed(const ndn::Name& name)
{
  NDN_LOG_INFO("ERROR: Failed to register prefix " << name << " in local hub's daemon");
}

} //mguard