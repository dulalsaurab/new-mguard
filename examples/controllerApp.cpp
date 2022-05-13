#include <controller/controller.hpp>
#include <iostream>

int main()
{
  // init controller
  std::string availableStreamsFilePath = "policies/available_streams";
  // std::string policyFilePath = "../policiespolicy1";
  std::string aaPrefix = "/ndn/org/md2k/mguard/aa";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix, availableStreamsFilePath);
  
  mGuardController.run();
  return 0;
}