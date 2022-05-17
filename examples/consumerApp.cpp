#include <user/subscriber.hpp>
#include <common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <time.h>

using namespace ndn::time_literals;

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
      std::cout << "received data: " << a << std::endl;
  }

  void
  processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams)
  {
    // check for convergence.
    m_subscriber.checkConvergence();

    // stop the process event
    m_subscriber.stop();

    std::cout << "\n\nStreams available for subscription" << std::endl;
    std::vector<ndn::Name> availableStreams, subscriptionList;
    int counter=0;
    if (streams.size() <= 0)
    {
      std::cout << "No eligible stream found for your policy" << std::endl;
    }
    for (auto &a : streams)
    {
      std::cout << ++counter << ": " << a << std::endl;
      availableStreams.push_back(a);
    }

    // these codes are only for testing purposes
    // std::vector<int> input;
    // std::cout << "enter selection, enter any char to stop" << std::endl;
    // while(!std::cin.fail())
    // {
    //     int value;
    //     std::cin >> value;
    //     if(!std::cin.fail())
    //       input.push_back(value);
    // }

    //random stream number
    std::vector<int> input;
    srand(time(NULL)); // Seed the time
    int random = 1 + (rand() % streams.size());
    input.push_back(random);
  

    std::cout << "\n" << std::endl;
    std::cout << "Subscribed to the stream/s" << std::endl;
    for (auto k : input)
    {
      auto ind = k-1;
      std::cout << k << ": " << availableStreams[ind] << std::endl;
      if (availableStreams[ind] != "/") // todo: fix this
        subscriptionList.push_back(availableStreams[ind]);
    }
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
main ()
{
  ndn::Name consumerPrefix = "/ndn/org/md2k/A";
  ndn::Name syncPrefix = "/ndn/org/md2k";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  std::string consumerCertPath = "certs/A.cert";
  std::string aaCertPath = "certs/aa.cert";
  mGuardConsumer consumer (consumerPrefix, syncPrefix, controllerPrefix, consumerCertPath, aaCertPath);
  consumer.handler();
}
