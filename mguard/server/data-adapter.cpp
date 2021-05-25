#include "data-adapter.hpp"

#include <ndn-cxx/util/logger.hpp>

#include <iostream>

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard
{

DataAdapter::DataAdapter(ndn::Face& face, const ndn::Name& syncPrefix,
                         const ndn::Name& userPrefix,
                         ndn::time::milliseconds syncInterestLifetime)

 : m_producer(face, syncPrefix, userPrefix, syncInterestLifetime,
              std::bind(&DataAdapter::processSyncUpdate, this, _1))
{
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
                           std::bind(&DataAdapter::registrationFailed, this, _1));
}


void
DataAdapter::processInterest(const ndn::Name& name, const ndn::Interest& interest)
{
  // check if the interest is for mainfest or data.
}

void
DataAdapter::onRegistrationSuccess(const ndn::Name& name)
{
  NDN_LOG_DEBUG("Successfully registered prefix: " << name);
}

void
DataAdapter::registrationFailed(const ndn::Name& name)
{
  NDN_LOG_ERROR("ERROR: Failed to register prefix " << name << " in local hub's daemon");
}

void
DataAdapter::processSyncUpdate(const std::vector<mguard::producer::SyncDataInfo> & syncInfo)
{
}

void
DataAdapter::sendData(const ndn::Name& name)
{}

} //mguard