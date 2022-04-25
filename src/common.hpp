#ifndef MGUARD_COMMON_HPP
#define MGUARD_COMMON_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <list>

namespace mguard {

namespace tlv
{

enum 
{
  mGuardContent = 127,
  mGuardPublisher = 128,
  mGuardController = 129,
  mGuardControllerKey = 130
};

}

const std::string SEMANTIC_LOCATION = "org.md2k--mguard--00000000-e19c-3956-9db2-5459ccadd40c--data_analysis--gps_episodes_and_semantic_location";

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};


const std::string DATA_DIR = "data";

struct AttributeMappingTable
{
  std::string source, columnInSource;
  std::vector<std::string> appliedTo; //streams id
};

}

#endif // MGUARD_COMMON_HPP
