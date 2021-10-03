#ifndef MGUARD_STREAM_HPP
#define MGUARD_STREAM_HPP

#include <ndn-cxx/name.hpp>
#include <string>

namespace mguard {
namespace util {

class Stream
{
public:
  Stream(const ndn::Name& name, std::vector<std::string>& attributeSet, const std::string& path);

  ndn::Name&
  getName()
  {
    return m_name;
  }

	std::string&
	getMD2KName()
  {
    return m_md2kName;
  }

	ndn::Name&
	getManifestName(ndn::Name manifestName)
  {
    return m_manifestName;
  }

	void
	setAttributes(std::vector<std::string>& attributeSet)
  {
      m_attributeSet = attributeSet;
  }

  std::vector<std::string>&
  getAttributes()
  {
    return m_attributeSet;
  }

  void
  setStreamDataPath(std::string path)
  {
    m_streamDataPath = path;
  }

  std::string&
  getStreamDataPath()
  {
    return m_streamDataPath;
  }

private:
  std::string m_md2kName;
  ndn::Name m_name;
  ndn::Name m_manifestName;
  std::vector<std::string> m_attributeSet;
  std::string m_streamDataPath;
};
} // util
} // mguard


#endif //MGUARD_STREAM_HPP