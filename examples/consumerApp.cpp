#include <user/subscriber.hpp>
#include <common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <string>
#include <sstream>
#include <iostream>

using namespace ndn::time_literals;

class mGuardConsumer
{
public:

  mGuardConsumer()
  : m_subscriber("/org/md2k/A", "/org/md2k", 1600_ms,
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
    std::vector<int> input; //
    std::cout << "enter selection, enter any char to stop" << std::endl;
    while(!std::cin.fail())
    {
        int value;
        std::cin >> value;
        if(!std::cin.fail())
          input.push_back(value);
    }
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
  // std::vector<std::string> subscriptionList {"/org/md2k/mguard/dd40c/phone/gps"};
  mGuardConsumer consumer;
  consumer.handler();
}