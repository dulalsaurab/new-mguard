#ifndef MGUARD_COMMON_HPP
#define MGUARD_COMMON_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <list>

#include <ndn-cxx/face.hpp>

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

// manifest ---------
/*
if use_manifest is set to false, manifest will not be used, application data will be publised directly.
*/
const bool USE_MANIFEST = true;

// manifest will be published after receiving 10 data units
const int MANIFEST_BATCH_SIZE = 10;

// if next update is not received withing 200 ms, the manifest will be publised, this can override batch size
const ndn::time::milliseconds MAX_UPDATE_WAIT_TIME(200); //todo: not implemented yet

// manifest ---------

const std::string SEMANTIC_LOCATION = "org.md2k--mguard--00000000-e19c-3956-9db2-5459ccadd40c--data_analysis--gps_episodes_and_semantic_location";

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

const std::string DATA_DIR = "data";
const uint64_t NOT_AVAILABLE = -1;
const uint64_t STARTING_SEQ_NUM = 1;

struct AttributeMappingTable
{
  std::string source, columnInSource;
  std::vector<std::string> appliedTo; //streams id
};

}

#endif // MGUARD_COMMON_HPP
