#include "data-adapter.hpp"
#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/encoding/block-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>

#include <iostream>
#include <string>

#include "common.hpp"

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard
{

DataAdapter::DataAdapter(const ndn::Name& attrAuthorityPrefix, const ndn::Name& producerPrefix)
: m_attrAuthorityPrefix(attrAuthorityPrefix)
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity("/mguard/producer").getDefaultKey().getDefaultCertificate())
, m_authorityCert(m_keyChain.getPib().getIdentity("/mguard/aa").getDefaultKey().getDefaultCertificate())
, m_kpAttributeAuthority(m_authorityCert, m_face, m_keyChain)
, m_producer(m_face, m_keyChain, m_producerCert, m_authorityCert)
{
}

bool 
DataAdapter::readData(std::vector<std::string> streamNames)
{
  for (auto name: streamNames)
  {
    auto mGuardName = name;
    std::replace(mGuardName.begin(), mGuardName.end(), '/', '-'); // converting name to mguard name
    auto streamPath = DATA_DIR + "/" + mGuardName;
    makeDataContent(m_fileProcessor.readStream(streamPath), name);
  }
}

ndn::Name
DataAdapter::makeDataName(ndn::Name streamName, std::string timestamp)
{
  return streamName.append(timestamp);
}

void
DataAdapter::makeDataContent(std::vector<std::string>data, ndn::Name streamName)
{
  for (auto row : data)
  {
    std::shared_ptr<ndn::Data> drow, ckData;
    ndn::Data d_row;
    m_tempRow = row;
    std::string delimiter = ",";
    auto timestamp = m_tempRow.substr(0, m_tempRow.find(delimiter));
    auto dataName = makeDataName(streamName, timestamp);
    d_row.setName(dataName);
    d_row.setContent(wireEncode());
    m_keyChain.sign(d_row);
    const uint8_t PLAIN_TEXT[1024] = {1};
    // std::tie(drow, ckData) = m_producer.produce(dataName, "attr1 or attr2", PLAIN_TEXT, sizeof(PLAIN_TEXT));
  }
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

// void
// DataAdapter::run()
// {
//   try {
//     m_face.processEvents();
//   }
//   catch (const std::exception& ex)
//   {
//     NDN_THROW(Error(ex.what()));
//     NDN_LOG_ERROR("Face error: " << ex.what());
//   }
// }

// void
// DataAdapter::stop()
// {
//   NDN_LOG_DEBUG("Shutting down face: ");
//   m_face.shutdown();
//   // m_face.getIoService().stop();
// }

// void
// DataAdapter::setInterestFilter(const ndn::Name& name, const bool loopback)
// {
//   NDN_LOG_INFO("Setting interest filter on: " << name);
//   m_face.setInterestFilter(ndn::InterestFilter(name).allowLoopback(false),
//                            std::bind(&DataAdapter::processInterest, this, _1, _2),
//                            std::bind(&DataAdapter::onRegistrationSuccess, this, _1),
//                            std::bind(&DataAdapter::registrationFailed, this, _1));
// }


// void
// DataAdapter::processInterest(const ndn::Name& name, const ndn::Interest& interest)
// {
//   // check if the interest is for mainfest or data.
// }

// void
// DataAdapter::onRegistrationSuccess(const ndn::Name& name)
// {
//   NDN_LOG_DEBUG("Successfully registered prefix: " << name);
// }

// void
// DataAdapter::registrationFailed(const ndn::Name& name)
// {
//   NDN_LOG_ERROR("ERROR: Failed to register prefix " << name << " in local hub's daemon");
// }

// void
// DataAdapter::processSyncUpdate(const std::vector<mguard::producer::SyncDataInfo> & syncInfo)
// {
// }

// void
// DataAdapter::sendData(const ndn::Name& name)
// {}

} //mguard