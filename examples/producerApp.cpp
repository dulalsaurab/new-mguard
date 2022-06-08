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

  mGuardProducer(ndn::Name& producerPrefix, const std::string& producerCertPath,
                 ndn::Name& aaPrefix, const std::string& aaCertPath,
                 const std::string& dbname)
  : m_scheduler(m_face.getIoService())
  , m_dataAdaptor(m_face, producerPrefix, producerCertPath, aaPrefix, aaCertPath, dbname)
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
  ndn::Name producerPrefix = "/ndn/org/md2k";
  ndn::Name aaPrefix = "/ndn/org/md2k/mguard/aa";
  std::string dbname = "lookup.db";
  std::string aaCertPath = "certs/aa.cert";
  std::string producerCertPath = "certs/producer.cert";

  std::map<std::string, std::string> datasetPath;
  std::string path = "/home/map901/mguard/mguard/data-generator";
  datasetPath.emplace("ndn--org--md2k--mguard--dd40c--phone--battery", path);
  datasetPath.emplace("ndn--org--md2k--mguard--dd40c--phone--gps", path);
  datasetPath.emplace("ndn--org--md2k--mguard--dd40c--data_analysis--gps_episodes_and_semantic_location", path);

  // mGuardProducer producerApp (producerPrefix, producerCertPath, aaPrefix, aaCertPath, dbname, datasetPath);
  mGuardProducer producerApp (producerPrefix, producerCertPath, aaPrefix, aaCertPath, dbname);
  producerApp.handler();
}
