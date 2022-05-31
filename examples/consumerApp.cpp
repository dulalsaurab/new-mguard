#include <user/subscriber.hpp>
#include <common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <string>
#include <sstream>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.examples.consumerApp);

static void
usage(const boost::program_options::options_description& options)
{
  NDN_LOG_INFO("Usage: ndnsd-consumer [options] e.g. printer \n" << options);
   exit(2);
}

class mGuardConsumer
{
public:

 mGuardConsumer(ndn::Name& consumerPrefix, ndn::Name& syncPrefix, ndn::Name& controllerPrefix,
                std::string& consumerCertPath, std::string& aaCertPath)
 : m_subscriber(consumerPrefix, syncPrefix, controllerPrefix,
                 consumerCertPath, aaCertPath, 1600_ms,
                 std::bind(&mGuardConsumer::processDataCallback, this, _1),
                 std::bind(&mGuardConsumer::processSubscriptionCallback, this, _1))
  {
  }

  void
  processDataCallback(const std::vector<std::string>& updates)
  {
    for (auto &a : updates)
      NDN_LOG_INFO("received data: " << a);
  }

  void
  processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams)
  {
    // check for convergence.
    m_subscriber.checkConvergence();

    // stop the process event
    m_subscriber.stop();

    NDN_LOG_INFO("\n\nStreams available for subscription");
    std::vector<ndn::Name> availableStreams, subscriptionList;
    int counter=0;
    if (streams.size() <= 0)
    {
      NDN_LOG_INFO("No eligible stream found for your policy");
    }
    for (auto &a : streams)
    {
      NDN_LOG_INFO(++counter << ": " << a);
      availableStreams.push_back(a);
    }

    // these codes are only for testing purposes
    // automatically subscriber to the respective streams

    // A. battery only 
    subscriptionList.push_back(availableStreams[0]); // battery 

    // all stream
    // subscriptionList.push_back(availableStreams[0]); // battery
    // subscriptionList.push_back(availableStreams[1]); // semloc
    // subscriptionList.push_back(availableStreams[3]); // gps

    // not gps
    // subscriptionList.push_back(availableStreams[0]); // battery
    // subscriptionList.push_back(availableStreams[2]); // sem_loc

    // accelerometer
    // subscriptionList.push_back(availableStreams[0]); // battery 

    // only work
    // subscriptionList.push_back(availableStreams[0]); // gps, only the one with attribute work should be accessible

    for (auto& s: subscriptionList)
      NDN_LOG_INFO("Subscribed to the stream/s" << s);

    // uncomment if: taking input from user ----------------------------------------------
    
    // std::vector<int> input; //
    // std::cout << "enter selection, enter any char to stop" << std::endl;
    // while(!std::cin.fail())
    // {
    //     int value;
    //     std::cin >> value;
    //     if(!std::cin.fail())
    //       input.push_back(value);
    // }
    // std::cout << "\n" << std::endl;
    // std::cout << "Subscribed to the stream/s" << std::endl;
    // for (auto k : input)
    // {
    //   auto ind = k-1;
    //   std::cout << k << ": " << availableStreams[ind] << std::endl;
    //   if (availableStreams[ind] != "/") // todo: fix this
    //     subscriptionList.push_back(availableStreams[ind]);
    // }
   
    // taking input from user end ----------------------------------------------

    m_subscriber.setSubscriptionList(subscriptionList);
    // run the processevent again, this time with sync as well
    m_subscriber.run(true);

  }

  void
  handler()
  {
    m_subscriber.run();
  }

private:
  ndn::Face m_face;
  mguard::subscriber::Subscriber m_subscriber;
};

int
main(int argc, char* argv[])
{

  std::string applicationPrefix;
  std::string certPath;

  namespace po = boost::program_options;
  po::options_description visibleOptDesc("Options");

  visibleOptDesc.add_options()
    ("help,h",      "print this message and exit")
    ("applicationPrefix,p", po::value<std::string>(&applicationPrefix)->required(), "application prefix, this name needs to match the one controller has")
    ("certificatePath,c", po::value<std::string>(&certPath), " location of consumer certificate")
  ;
  
  try
  {
    po::variables_map optVm;
    po::store(po::parse_command_line(argc, argv, visibleOptDesc), optVm);
    po::notify(optVm);

    if (optVm.count("applicationPrefix")) {
      if (applicationPrefix.empty())
      {
        std::cerr << "ERROR: applicationPrefix cannot be empty" << std::endl;
        usage(visibleOptDesc);
      }
    }
    if (optVm.count("certificatePath")) {
      if (certPath.empty())
      {
        std::cerr << "ERROR: certificatePath cannot be empty" << std::endl;
        usage(visibleOptDesc);
      }
    }

  }
  catch (const po::error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    usage(visibleOptDesc);
  }

  ndn::Name consumerPrefix(applicationPrefix);
  ndn::Name syncPrefix = "/ndn/org/md2k";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  // std::string consumerCertPath = "certs/A.cert";
  std::string aaCertPath = "certs/aa.cert";
  mGuardConsumer consumer (consumerPrefix, syncPrefix, controllerPrefix, certPath, aaCertPath);
  consumer.handler();
}
