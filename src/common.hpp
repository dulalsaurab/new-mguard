#ifndef MGUARD_COMMON_HPP
#define MGUARD_COMMON_HPP

#include <ndn-cxx/util/io.hpp>
#include <ndn-cxx/face.hpp>

#include <iostream>
#include <vector>
#include <utility>
#include <list>

namespace mguard {


inline
std::shared_ptr<ndn::security::Certificate>
loadCert(const std::string& certLoc)
{
  std::ifstream input_file(certLoc);
  std::string certStr((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
  std::istringstream ss(certStr);
  auto cert = ndn::io::load<ndn::security::Certificate>(ss);
  return cert;
}

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

// get ndn name for semantic location name

// manifest ---------
/*
if use_manifest is set to false, manifest will not be used, application data will be publised directly.
*/
const bool USE_MANIFEST = true;

// manifest will be published after receiving 10 data units
const int MANIFEST_BATCH_SIZE = 50;

// if next update is not received withing 200 ms, the manifest will be publised, this can override batch size
const ndn::time::milliseconds MAX_UPDATE_WAIT_TIME(200); //todo: not implemented yet

// manifest ---------
const std::string SEMANTIC_LOCATION = "ndn--org--md2k--mguard--dd40c--data_analysis--gps_episodes_and_semantic_location";
const std::string NDN_LOCATION_STREAM = "/ndn/org/md2k/mguard/dd40c/phone/gps";
const std::string NDN_BATTERY_STREAM = "/ndn/org/md2k/mguard/dd40c/phone/battery";


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
