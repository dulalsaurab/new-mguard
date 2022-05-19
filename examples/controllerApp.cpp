#include <controller/controller.hpp>
#include <iostream>

int main()
{
  // init controller

//moved the policies and cert folder to home directory for testing  in this case the home directory was /home/vagrant
  std::string availableStreamsFilePath = "/home/vagrant/policies/available_streams";
  std::string policyFilePath = "/home/vagrant/policies/policy1";
  std::string aaPrefix = "/ndn/org/md2k/mguard/aa";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  std::string aaCertPath = "/home/vagrant/certs/aa.cert";
  std::vector<std::string> policyList = {"/home/vagrant/policies/policy1"};

  std::map<ndn::Name, std::string> requesterCertMap;
  requesterCertMap.emplace("/ndn/org/md2k/A", "/home/vagrant/certs/A.cert");
 
  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix,
                                                  aaCertPath, requesterCertMap,
                                                  availableStreamsFilePath,
                                                  policyList);

  mGuardController.run();
  return 0;
}
