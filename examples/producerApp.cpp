#include "server/data-adapter.hpp"
#include "server/util/stream.hpp"
#include "common.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <chrono>
#include <unistd.h>
#include <thread>

struct streamInfo
{
  std::string streamName;
  std::vector<std::string> attributes;
  std::string dataPath;
};

class mGuardProducer
{
public:

  mGuardProducer()
  : m_scheduler(m_face.getIoService())
  , m_dataAdaptor(m_face, "/org/md2k", "/mguard/aa", "lookup.db")
  {
  }
  
 void
 handler()
 {
  m_dataAdaptor.run();
 }

private:
  ndn::Face m_face;
  ndn::Scheduler m_scheduler;
  mguard::DataAdapter m_dataAdaptor;
};


int main ()
{
  mGuardProducer producerApp;
  producerApp.handler();
}
