#include <controller/controller.hpp>
#include <iostream>

int main()
{
  // init controller
  std::string availableStreamsFilePath = "policies/available_streams";
  // std::string policyFilePath = "../policiespolicy1";
  std::string aaPrefix = "/ndn/org/mguard/aa";
  ndn::Name controllerPrefix = "/ndn/org/mguard/controller";
  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix, availableStreamsFilePath);
  
  mGuardController.run();
  return 0;
}