#include <user/subscriber.hpp>
#include <server/common.hpp>

#include <iostream>

using namespace ndn::time_literals;

class mGuardConsumer
{
public:

  mGuardConsumer()
  : m_subscriber("/org.md2k", 1600_ms, std::bind(&mGuardConsumer::processCallback, this, _1))
  {
  }

  void
  processCallback(const std::vector<mguard::SyncDataInfo>& updates)
  {
    std::cout << "just a reply" << std::endl;
  }

private:
  mguard::subscriber::Subscriber m_subscriber;
};

int 
main ()
{
  return 0;  
}