#include "common.hpp"
#include "data-adapter.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/encoding/block-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>

#include <iostream>
#include <string>

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard
{

DataAdapter::DataAdapter(ndn::Face& face, ndn::security::KeyChain& keyChain, const ndn::Name& attrAuthorityPrefix, 
                                            const ndn::Name& producerPrefix)
: m_face(face)
, m_keyChain(keyChain)
, m_attrAuthorityPrefix(attrAuthorityPrefix)
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity(m_producerPrefix).getDefaultKey().getDefaultCertificate())
, m_authorityCert(m_keyChain.getPib().getIdentity(m_attrAuthorityPrefix).getDefaultKey().getDefaultCertificate())
, m_kpAttributeAuthority(m_authorityCert, m_face, m_keyChain)
, m_producer(m_face, m_keyChain, m_producerCert, m_authorityCert)
{
}

void
DataAdapter::readData(util::Stream& stream)
{
  // processing individual stream
  NDN_LOG_INFO("Processing stream: " << stream.getName());
  makeDataContent(m_fileProcessor.readStream(stream.getStreamDataPath()), stream);
 
  // set interest filter in stream name, any interest for data corresponding to this stream will be served
  setInterestFilter(stream.getName());
  // need to send call back
}

ndn::Name
DataAdapter::makeDataName(ndn::Name streamName, std::string timestamp)
{
  return streamName.append("DATA").append(timestamp);
}

bool
DataAdapter::makeDataContent(std::vector<std::string>data, util::Stream& stream)
{
  for (auto row : data)
  {
    // TODO: create a manifest, and append each <data-name>/<implicit-digetst> to the manifest
    std::shared_ptr<ndn::Data> enc_data, ckData;

    // get timestamp from the data row
    std::string delimiter = ",";
    auto timestamp = m_tempRow.substr(0, m_tempRow.find(delimiter));
    auto dataName = makeDataName(stream.getName(), timestamp);
    try
    {
      NDN_LOG_DEBUG("Encrypting data: " << dataName);
      unsigned char* byteptr = reinterpret_cast<unsigned char *>(&row);
      std::tie(enc_data, ckData) = m_producer.produce(dataName, stream.getAttributes(), byteptr, sizeof(byteptr));
      NDN_LOG_INFO("here");
    }
    catch(const std::exception& e)
    {
      NDN_LOG_ERROR("Encryption failled");
      std::cerr << e.what() << '\n';
      return false;
    }
    //  encrypted data is created, store it in the buffer and publish it
    NDN_LOG_INFO("data: " << enc_data << " ckData: " << ckData);
    m_dataBuffer.emplace(dataName, enc_data);
  }
  return true;
  // data for the given stream is ready, send notification interest
  // sendNotificationInterest();
}

const ndn::Block &
DataAdapter::wireEncode()
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
DataAdapter::wireEncode(ndn::EncodingImpl<TAG> &encoder) const
{
  size_t totalLength = 0;
  totalLength += prependStringBlock(encoder, tlv::DataRow, m_tempRow);
  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::mGuardContent);

  return totalLength;
}

// Data adaptor only communicates with REPO, consumers interest should either go to publisher or repo directly
void
DataAdapter::run()
{
  try {
    m_face.processEvents();
  }
  catch (const std::exception& ex)
  {
    NDN_THROW(Error(ex.what()));
    NDN_LOG_ERROR("Face error: " << ex.what());
  }
}

void
DataAdapter::stop()
{
  NDN_LOG_DEBUG("Shutting down face: ");
  m_face.shutdown();
  // m_face.getIoService().stop();
}

void
DataAdapter::setInterestFilter(const ndn::Name& name, const bool loopback)
{
  NDN_LOG_INFO("Setting interest filter on: " << name);
  m_face.setInterestFilter(ndn::InterestFilter(name).allowLoopback(false),
                           std::bind(&DataAdapter::processInterest, this, _1, _2),
                           std::bind(&DataAdapter::onRegistrationSuccess, this, _1),
                           std::bind(&DataAdapter::onRegistrationFailed, this, _1));
}


void
DataAdapter::processInterest(const ndn::Name& name, const ndn::Interest& interest)
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
DataAdapter::sendApplicationNack(const ndn::Name& name)
{
  NDN_LOG_DEBUG("Sending application nack");
  ndn::Name dataName(name);
  ndn::Data data(dataName);
  data.setContentType(ndn::tlv::ContentType_Nack);
  m_keyChain.sign(data);
  m_face.put(data);
}


void
DataAdapter::onRegistrationSuccess(const ndn::Name& name)
{
  NDN_LOG_DEBUG("Successfully registered prefix: " << name);
}

void
DataAdapter::onRegistrationFailed(const ndn::Name& name)
{
  NDN_LOG_ERROR("ERROR: Failed to register prefix " << name << " in local hub's daemon");
}

} //mguard