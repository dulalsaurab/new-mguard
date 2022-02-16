#include <user/subscriber.hpp>
#include <common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <iostream>

using namespace ndn::time_literals;

class mGuardConsumer
{
public:

  mGuardConsumer(std::vector<std::string>& subscriptionList)
  : m_subscriber("/org/md2k/A", "/org/md2k", 1600_ms, subscriptionList, 
                 std::bind(&mGuardConsumer::processCallback, this, _1))
  {
  }

  void
  processCallback(const std::vector<mguard::SyncDataInfo>& updates)
  {
    std::cout << "just a reply" << std::endl;
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
  std::vector<std::string> subscriptionList {"/org/md2k/mguard/dd40c/gps/phone"};
  mGuardConsumer consumer (subscriptionList);
  consumer.handler();
}