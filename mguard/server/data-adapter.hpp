
#ifndef MGUARD_DATA_ADAPTER_HPP
#define MGUARD_DATA_ADAPTER_HPP

#include "publisher.hpp"

#include <ndn-cxx/face.hpp>
#include <PSync/full-producer.hpp>

namespace mguard
{

/*
test streams
/org.md2k/studyA/user-id/LOCATION/PHONE
/org.md2k/studyA/00000000-e19c-3956-9db2-5459ccadd40c/ACCELEROMETER/PHONE
/org.md2k/studyA/00000000-e19c-3956-9db2-5459ccadd40c/GYROSCOPE/PHONE
/org.md2k/studyA/00000000-e19c-3956-9db2-5459ccadd40c/ACCELEROMETER/MOTION_SENSE/LEFT_WRIST
/org.md2k/studyA/00000000-e19c-3956-9db2-5459ccadd40c/data_analysis/gps_episodes_and_semantic_location

*/


class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class DataAdapter
{

public:
  DataAdapter(ndn::Face& facePtr, const ndn::Name& syncPrefix,
              const ndn::Name& userPrefix,
              ndn::time::milliseconds syncInterestLifetime);

  void
  run();

  void
  stop();

  void
  processSyncUpdate(const std::vector<mguard::publisher::SyncDataInfo> & syncInfo);

private:
  void
  processInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  sendData(const ndn::Name& name);

  void
  setInterestFilter(const ndn::Name& prefix, const bool loopback = false);

  void
  registrationFailed(const ndn::Name& name);

  void
  onRegistrationSuccess(const ndn::Name& name);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block, const std::string& info) const;

  const ndn::Block&
  wireEncode();


private:
  publisher::Publisher m_producer;
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
};

}

#endif // MGUARD_DATA_ADAPTER_HPP