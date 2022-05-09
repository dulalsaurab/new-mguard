#include "controller.hpp"

using namespace ndn;

namespace mguard {
namespace controller {

NDN_LOG_INIT(mguard.Controller);

Controller::Controller(const ndn::Name& controllerPrefix, const ndn::Name& aaPrefix, 
                       const std::string& availableStreamsFilePath)
: m_controllerPrefix(controllerPrefix)
, m_aaPrefix(aaPrefix)
, m_policyParser(availableStreamsFilePath)
, m_aaCert(m_keyChain.getPib().getIdentity(m_aaPrefix.toUri()).getDefaultKey().getDefaultCertificate())
, m_attrAuthority(m_aaCert, m_face, m_keyChain) 
{
  // TODO: list the policy path into mGuard configuration file or in the common.hpp, and process all the streams
  std::vector<std::string> policyList = {
    // "/home/map901/mguard/mguard/policies/policy5",
                                           "/home/map901/mguard/mguard/policies/policy1",
                                           "/home/map901/mguard/mguard/policies/policy2"
                                          //  "/home/map901/mguard/mguard/policies/policy3",
                                          //  "/home/map901/mguard/mguard/policies/policy4"
                                          };

  for(auto& policy : policyList)
  {
    NDN_LOG_INFO("policy path: " << policy);
    processPolicy(policy);
  }
  for(auto& it : m_policyMap)
    NDN_LOG_DEBUG("username: " << it.first << " ABE policy: " << it.second.abePolicy);
  
  setInterestFilter(m_controllerPrefix);
}

void
Controller::run()
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
Controller::processPolicy(const std::string& policyPath)
{
  m_policyParser.inputPolicy(policyPath);
  auto policyDetail = m_policyParser.getPolicyInfo();
  NDN_LOG_DEBUG("from policy info: " << policyDetail.abePolicy);

  // TODO: modify parser to store streams as ndn Name not the strings
  // in doing so we don't need the following conversion
  std::list <ndn::Name> tempStreams;
  for (const std::string& name : policyDetail.streams)
  {
    NDN_LOG_TRACE("Streams got from parser: " << name);
    tempStreams.push_back(name);
  }

  for (const std::string& requester : policyDetail.requesters) {
    NDN_LOG_DEBUG("Getting key and storing policy details for user: " << requester);

    try
    {
      auto requesterCert = m_keyChain.getPib().getIdentity(requester).getDefaultKey().getDefaultCertificate();
      NDN_LOG_DEBUG ("ABE policy for policy id: " << policyDetail.policyIdentifier << ": " << policyDetail.abePolicy);
      m_attrAuthority.addNewPolicy(requesterCert, policyDetail.abePolicy);
      policyDetails policyD = {policyDetail.policyIdentifier, tempStreams, policyDetail.abePolicy};
      m_policyMap.insert(std::pair <ndn::Name, policyDetails> (requester, policyD));
    }
    catch (std::exception& ex)
    {
      NDN_LOG_ERROR(ex.what());
      NDN_LOG_DEBUG("Error getting the cert, requester cert might be missing");
    }
  }
}

// /data/receive  ---- interest on this prefix from external application, data generator. 
void
Controller::setInterestFilter(const ndn::Name& name, const bool loopback)
{
  NDN_LOG_INFO("Setting interest filter on: " << name);
  m_face.setInterestFilter(ndn::InterestFilter(name).allowLoopback(false),
                           std::bind(&Controller::processInterest, this, _1, _2),
                           std::bind(&Controller::onRegistrationSuccess, this, _1),
                           std::bind(&Controller::onRegistrationFailed, this, _1));
}

void
Controller::processInterest(const ndn::Name& name, const ndn::Interest& interest)
{
  NDN_LOG_INFO("Interest received: " << interest.getName() << " name: " << name);
  // TODO: consumer will sent a signed interest, name will be extracted from identity 
  // extract subscriber name from the interest
  auto subscriberName = interest.getName().getSubName(2);
  NDN_LOG_INFO("Consumer name: " << subscriberName);
  sendData(interest.getName());
}

void
Controller::sendData(const ndn::Name& name)
{
  auto subscriberName = name.getSubName(2);

  ndn::Data replyData(name);
  // replyData.setFreshnessPeriod(5_s);

  auto it = m_policyMap.find(subscriberName);
  if (it == m_policyMap.end()) {
    NDN_LOG_INFO("Key for subscriber: " << subscriberName << " not found " << "sending NACK");
    sendApplicationNack(name);
  }
  m_temp_policyDetail = it->second;
  replyData.setContent(wireEncode());
  m_keyChain.sign(replyData);
  m_face.put(replyData);
  NDN_LOG_DEBUG("Data sent for :" << name);
}

void
Controller::onRegistrationSuccess(const ndn::Name& name)
{
  NDN_LOG_INFO("Successfully registered prefix: " << name);
}

void
Controller::onRegistrationFailed(const ndn::Name& name)
{
  NDN_LOG_INFO("ERROR: Failed to register prefix " << name << " in local hub's daemon");
}

void
Controller::sendApplicationNack(const ndn::Name& name)
{
  NDN_LOG_INFO("Sending application nack");
  ndn::Name dataName(name);
  ndn::Data data(dataName);
  data.setContentType(ndn::tlv::ContentType_Nack);

  m_keyChain.sign(data);
  m_face.put(data);
}

const ndn::Block&
Controller::wireEncode()
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
Controller::wireEncode(ndn::EncodingImpl<TAG> &encoder)
{
  size_t totalLength = 0;
  auto& accessibleStreams = m_temp_policyDetail.streams;

  for (auto it = accessibleStreams.rbegin(); it != accessibleStreams.rend(); ++it) {
    NDN_LOG_DEBUG (" Encoding stream name: " << *it);
    totalLength += it->wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(mguard::tlv::mGuardController);
  
  return totalLength;
}

} // namespace controller
} // namespace mguard
