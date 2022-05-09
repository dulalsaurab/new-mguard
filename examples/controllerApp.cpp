#include <controller/controller.hpp>
#include <iostream>

int main()
{
  // init controller
  std::string availableStreamsFilePath = "policies/available_streams";
  // std::string policyFilePath = "../policiespolicy1";
  std::string aaPrefix = "/mguard/aa";
  ndn::Name controllerPrefix = "/mguard/controller";
  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix, availableStreamsFilePath);
  
  mGuardController.run();
  return 0;
}