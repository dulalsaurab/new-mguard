#include "common.hpp"
#include "data-adapter.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/encoding/block-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <iostream>
#include <string>

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard
{

// DataAdapter::DataAdapter(const ndn::Name& producerPrefix,
//                          const ndn::security::Certificate& producerCert,
//                          const ndn::security::Certificate& attrAuthorityCertificate)

DataAdapter::DataAdapter(const ndn::Name& producerPrefix,
                         const ndn::Name& aaPrefix)
: m_keyChain("pib-sqlite3:/Users/sdulal/Documents/PROJECTS/mguard/certs/.ndn", "tpm-file")
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity(producerPrefix).getDefaultKey().getDefaultCertificate())
, m_ABE_authorityCert(m_keyChain.getPib().getIdentity(aaPrefix).getDefaultKey().getDefaultCertificate())
, m_publisher(m_face, m_keyChain, m_producerPrefix, m_producerCert, m_ABE_authorityCert)
{
  std::cout << m_ABE_authorityCert;
  std::cout << "--------------------" << std::endl;
  std::cout << m_producerCert;
}

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

ndn::Name
DataAdapter::makeDataName(ndn::Name streamName, std::string timestamp)
{
  return streamName.append("DATA").append(timestamp);
}

bool
DataAdapter::publishDataUnit(util::Stream& stream)
{
  NDN_LOG_INFO("Processing stream: " << stream.getName());
  auto dataSet = m_fileProcessor.readStream(stream.getStreamDataPath());

  for (auto data : dataSet)
  {
    NDN_LOG_TRACE("Data unit" << data);
    // get timestamp from the data row
    std::string delimiter = ",";
    m_tempRow = data;
    auto timestamp = m_tempRow.substr(0, m_tempRow.find(delimiter));
    auto dataName = makeDataName(stream.getName(), timestamp);
    NDN_LOG_DEBUG ("Publishing data name: " << dataName << " Timestamp: " << timestamp);
    m_publisher.publish(dataName, data, stream.getAttributes());
  }
}

} //mguard