#include <mguard/server/data-adapter.hpp>
#include <mguard/server/util/stream.hpp>
#include <mguard/server/common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/encoding/block-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
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

  mGuardProducer(mguard::util::Stream stream)
  : m_scheduler(m_face.getIoService())
  , m_stream(stream)
  , m_dataAdaptor(m_face, "/mguard/producer", "/mguard/aa") 
  {
  }
  
 void
 handler()
 {
  m_scheduler.schedule(ndn::time::milliseconds(2000), [=] {
    std::cout << "publishing stream" << std::endl;
    m_dataAdaptor.publishDataUnit(m_stream);
  });
  m_dataAdaptor.run();
 }

private:
  ndn::Face m_face;
  ndn::Scheduler m_scheduler;

  //  std::vector<mguard::util::Stream> m_streams;
  mguard::util::Stream m_stream;
  mguard::DataAdapter m_dataAdaptor;
};


int main ()
{
  streamInfo A;
  A.streamName = "/org.md2k/mguard/dd40c/gps/phone";
  A.attributes = {"org.md2k", "/org.md2k/mguard/dd40c/gps/phone"};
  A.dataPath = mguard::DATA_DIR + "/" + "org.md2k-mguard-dd40c-gps-phone.csv";

  mguard::util::Stream stream(A.streamName, A.attributes, A.dataPath);
  mGuardProducer producerApp(stream);
  producerApp.handler();


}
