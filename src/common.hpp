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
