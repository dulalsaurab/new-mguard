#include "stream.hpp"
#include "common.hpp"

#include <ndn-cxx/util/logger.hpp>

namespace mguard {
namespace util {

NDN_LOG_INIT(mguard.Stream);

/*
if use_manifest is set to false, manifest will not be used, application data will be publised directly.
*/
const bool USE_MANIFEST = true;

// manifest will be published after receiving 100 data units
const int MANIFEST_BATCH_SIZE = 4;

// if next update is not received withing 100 ms, the manifest will be publised, this can override batch size
const int MAX_UPDATE_WAIT_TIME = 100; //todo: not implemented yet

Stream::Stream(const std::string& md2kName, std::vector<std::string> attributeSet)
: m_md2kName(md2kName)
, m_attributeSet(attributeSet)
, m_manifestCounter (0)
{
  m_name = std::regex_replace(m_md2kName, std::regex("--"), "/"); // convert to ndn name

  m_manifestName = m_name;
  m_manifestName.append("manifest");

  m_attributeSet.push_back(m_name.toUri());

  NDN_LOG_DEBUG("stream name: " << m_md2kName);
  NDN_LOG_DEBUG("NDN name: " << m_name);
  NDN_LOG_DEBUG("Manifest name: " << m_manifestName);
}

bool
Stream::updateManifestList(const ndn::Name& dataNameWithDigest)
{
  m_manifestList.push_back(dataNameWithDigest);
  if (m_manifestCounter == MANIFEST_BATCH_SIZE - 1)
    {
      m_manifestCounter = 0; // reset the counter
      return true; // time to publish the manifest
    }
  
  ++m_manifestCounter;
  return false;
}

} // util
} // mguard