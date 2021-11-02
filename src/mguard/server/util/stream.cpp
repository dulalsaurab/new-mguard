#include "stream.hpp"
#include "../common.hpp"

namespace mguard {
namespace util {

Stream::Stream(const ndn::Name& name, std::vector<std::string>& attributeSet, const std::string& path)
: m_name(name)
, m_attributeSet(attributeSet)
, m_streamDataPath(path)
{
  std::string m_md2kName = m_name.toUri();
  std::replace(m_md2kName.begin(), m_md2kName.end(), '/', '-'); // converting name to mguard name
  m_manifestName = m_name;
  m_manifestName.append("manifest");
  // m_streamDataPath =  DATA_DIR + "/" + m_md2kName + ".csv";
}

} // util
} // mguard