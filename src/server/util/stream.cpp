#include "stream.hpp"
#include "common.hpp"

namespace mguard {
namespace util {

/*
if use_manifest is set to false, manifest will not be used, application data will be publised directly.
*/
const bool USE_MANIFEST = true;

// manifest will be published after receiving 100 data units
const int MANIFEST_BATCH_SIZE = 4;

// if next update is not received withing 100 ms, the manifest will be publised, this can override batch size
const int MAX_UPDATE_WAIT_TIME = 100; //todo: not implemented yet

Stream::Stream(const ndn::Name& name, std::vector<std::string>& attributeSet, const std::string& path)
: m_name(name)
, m_attributeSet(attributeSet)
, m_streamDataPath(path)
, m_manifestCounter (0)
{
  std::string m_md2kName = m_name.toUri();
  std::replace(m_md2kName.begin(), m_md2kName.end(), '/', '-'); // converting name to mguard name
  m_manifestName = m_name;
  m_manifestName.append("manifest");
  // m_streamDataPath =  DATA_DIR + "/" + m_md2kName + ".csv";
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