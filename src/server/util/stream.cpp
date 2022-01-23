#include "stream.hpp"
#include "../common.hpp"

namespace mguard {
namespace util {

int MANIFEST_BATCH_SIZE = 4;

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
Stream::updateManifestList(ndn::Name dataNameWithDigest)
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