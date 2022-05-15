#include "file-processor.hpp"
#include <ndn-cxx/util/logger.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

NDN_LOG_INIT(mguard.fileProcessor);

namespace mguard
{

std::vector<std::string>
FileProcessor::readStream(std::string streamPath)
{
  std::vector<std::string> data;
  std::string line;
  std::ifstream inputFile;
  inputFile.open(streamPath.c_str(), std::ios::in);
  if (!inputFile.is_open())
  {
    std::string msg = "Failed to read mapping file: ";
    msg += streamPath;
    std::cerr << msg << std::endl;
  }
  getline(inputFile, line);

  while (getline(inputFile, line)) {
    data.push_back(line);
  }
  inputFile.close();
  return data;
}

std::vector<std::string>
FileProcessor::getVectorByDelimiter(std::string _s, std::string delimiter)
{
  size_t pos = 0;
  std::string token;
  std::vector<std::string> _vec;
  while ((pos = _s.find(delimiter)) != std::string::npos)
  {
    token = _s.substr(0, pos);
    if (!(boost::algorithm::contains(token, "timestamp"))) { // skip the column that have timestamp in it
      _vec.push_back(token);
    }
    _s.erase(0, pos + delimiter.length());
  }
  NDN_LOG_TRACE("Remaining content of the stream: " << _s);
  if (!(_s.empty()))
    _vec.push_back(_s); // finally append the remaining string
  return _vec;
}

AttributeMappingFileProcessor::AttributeMappingFileProcessor(const std::string& filename)
: m_filename (filename)
{
}

bool
AttributeMappingFileProcessor::processAttributeMappingFile()
{
	bool ret = true;
	std::ifstream inputFile;
	inputFile.open(m_filename.c_str());
	if (!inputFile.is_open())
	{
		std::string msg = "Failed to read mapping file: ";
		msg += m_filename;
		std::cerr << msg << std::endl;
		return false;
	}
	try {
		MappingSection pt;
		boost::property_tree::read_info(inputFile, pt);

		for (const auto &tn : pt)
		{
			if (tn.first == "streams")
				ret = processStreamsSection(tn.second);
			else if (tn.first == "attribute-mapping")
				ret = processMappingSection(tn.second);
		}
	}
	catch (const boost::property_tree::info_parser_error &error)
	{
		std::cerr << "Failed to parse mapping  file " << std::endl;
		std::cerr << m_filename << std::endl;
		return false;
	}
  return ret;
}

bool
AttributeMappingFileProcessor::processStreamsSection(const MappingSection& section)
{
  for (auto& it: section)
  {
    auto streamName = section.get<std::string> (it.first);
    if (!streamName.empty()) {
      m_streams.emplace(it.first, streamName);
    }
    else {
      std::cout << "Stream name cannot be empty" << std::endl;
      return false;
    }
    std::cout << "id: " << it.first << " name : " << streamName << std::endl;
  }
  return true;
}

bool
AttributeMappingFileProcessor::processMappingSection(const MappingSection& section)
{
  bool ret;
  try {
    for (auto& it: section)
    {
      MappingSection pt;
      mguard::AttributeMappingTable temp;
      auto _sec = it.second;
      temp.source = _sec.get<std::string>("source");
      temp.columnInSource = _sec.get<std::string>("column");
      temp.appliedTo = getVectorByDelimiter(_sec.get<std::string>("applied_to"), ",");
      std::cout << "name: " << it.first << " source " << temp.source  << " column: " << temp.columnInSource << std::endl;
      if (!(temp.source.empty() || temp.columnInSource.empty() || temp.appliedTo.empty())) {
        m_mappingTable.emplace(it.first, temp);
      }
      else{
        std::cout << "source or columnInSource or appliedTo cannot be empty;" << std::endl;
        ret = false;
      }
    }
  }
  catch (const std::exception &ex)
  {
    std::cerr << ex.what() << std::endl;
    ret = false;
  }
  return ret;
}

} // mguard
