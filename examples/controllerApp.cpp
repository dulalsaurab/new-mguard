#include <controller/controller.hpp>
#include <iostream>

int main()
{
  // init controller
  std::string availableStreamsFilePath = "policies/available_streams";
  // std::string policyFilePath = "../policiespolicy1";
  std::string aaPrefix = "/ndn/org/md2k/mguard/aa";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  std::string aaCertPath = "certs/aa.cert";

  std::map<ndn::Name, std::string> requesterCertMap;
  requesterCertMap.emplace("/ndn/org/md2k/A", "certs/A.cert");
  requesterCertMap.emplace("/ndn/org/md2k/B", "certs/B.cert");
  requesterCertMap.emplace("/ndn/org/md2k/C", "certs/C.cert");
  requesterCertMap.emplace("/ndn/org/md2k/D", "certs/D.cert");

  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix,
                                                  aaCertPath, requesterCertMap,
                                                  availableStreamsFilePath);

  mGuardController.run();
  return 0;
}
