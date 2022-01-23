#include "common.hpp"
#include "data-adapter.hpp"

#include <iostream>
#include <string>

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard {

DataAdapter::DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix, const ndn::Name& aaPrefix)
: m_face(face)
, m_producerPrefix(producerPrefix)
, m_producerCert(m_keyChain.getPib().getIdentity(producerPrefix).getDefaultKey().getDefaultCertificate())
, m_ABE_authorityCert(m_keyChain.getPib().getIdentity(aaPrefix).getDefaultKey().getDefaultCertificate())
, m_publisher(m_face, m_keyChain, m_producerPrefix, m_producerCert, m_ABE_authorityCert)
{
  NDN_LOG_DEBUG("default identity" <<  m_keyChain.getPib().getDefaultIdentity());
  NDN_LOG_DEBUG ("Initialized data adaptor and publisher");
  NDN_LOG_DEBUG ("Producer cert: " << m_producerCert);
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("ABE authority cert: " << m_ABE_authorityCert);
  
}

void
DataAdapter::run()
{
  try {
    m_face.processEvents();
  }
  catch (const std::exception& ex)
  {
    NDN_LOG_ERROR("Face error: " << ex.what()); 
    NDN_THROW(Error(ex.what()));
  }
}

void
DataAdapter::stop()
{
  NDN_LOG_DEBUG("Shutting down face: ");
  m_face.shutdown();
  // m_face.getIoService().stop();
}

ndn::Name
DataAdapter::makeDataName(ndn::Name streamName, std::string timestamp)
{
  return streamName.append("DATA").append(timestamp);
}

void
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
    //TODO: need to change this, don't want to pass stream here, but rather just the attributes.
    m_publisher.publish(dataName, data, stream);
  }
}

} //mguard