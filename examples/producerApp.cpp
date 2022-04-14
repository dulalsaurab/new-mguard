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

  mGuardProducer(mguard::util::Stream stream)
  : m_face()
  , m_scheduler(m_face.getIoService())
  , m_stream(stream)
  , m_dataAdaptor(m_face, "/org/md2k", "/mguard/aa") 
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
  // boost::asio::io_service io_service;
  mguard::util::Stream m_stream;
  mguard::DataAdapter m_dataAdaptor;
};


int main ()
{


//   int main(int argc, char *argv[])
// {
  // try
  //   {
  // boost::asio::io_service io_service;
  // mguard::Server server(io_service);
  // io_service.run();
  //   // }
  // catch(std::exception& e)
  //   {
  //   std::cerr << e.what() << endl;
  //   }
  //  std::cout << "Hello world" << std::endl;
  // return 0;
// }


  streamInfo A;
  A.streamName = "/org/md2k/mguard/dd40c/phone/gps";
  A.attributes = {"/org/md2k/mguard/dd40c/phone/gps"};
  A.dataPath = mguard::DATA_DIR + "/" + "org-md2k-mguard-dd40c-phone-gps.csv";

  mguard::util::Stream stream(A.streamName, A.attributes, A.dataPath);
  mGuardProducer producerApp(stream);
  producerApp.handler();
}
