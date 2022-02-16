#include <controller/controller.hpp>
// #include <controller/parser.hpp>
#include <iostream>

int main()
{
  // init controller
  std::string availableStreamsFilePath = "policies/available_streams";
  std::string policyFilePath = "default.policy";
  std::string aaPrefix = "/mguard/aa";
  ndn::Name controllerPrefix = "/mguard/controller";
  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix, availableStreamsFilePath);
  
  // todo: make default.policy the default one unless given other arguments
  // std::string policyFilePath("../default.policy"), availableStreamsFilePath ("../policies/available_streams");

    // // only allowing -f policyFilePath to change policy file name
    // // very crude way of doing this, but it works for now
    // if (argc == 3) {
    //     std::string s = "-f";
    //     if ((s.compare(argv[1])) == 0) {
    //         policyFilePath = argv[2];
    //     } else {
    //         return -1;
    //     }
    // }

    // this createsa a parser with the path to the config file and the available_streams file
    // the resulting ABE policy is automatically generated
    // mguard::PolicyParser pp(policyFilePath, availableStreamsFilePath);

    // std::cout << pp << std::endl;

    mGuardController.run();
    return 0;

}