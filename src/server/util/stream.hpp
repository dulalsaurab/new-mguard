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

  /*
   This function will update the manifest list and send status whether it is reedy to be published or not
   e.g. if 20 new data names are added or X ms/s time has passed since addition of the last data poing,
   manifest will be read to be published
   for time expiration, we need to use signal ??
  */
  bool
  updateManifestList(ndn::Name dataNameWithDigest);

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
	getManifestName()
  {
    return m_manifestName;
  }

	void
	setAttributes(std::vector<std::string>& attributeSet)
  {
      m_attributeSet = attributeSet;
  }

  // TODO: data points from a same stream can be encrypted with different set 
  // of attribtues, need to consider it in the future
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
  std::vector<ndn::Name> m_manifestList;
  int m_manifestCounter;

};
} // util
} // mguard


#endif //MGUARD_STREAM_HPP