#ifndef MGUARD_FILE_PROCESSOR_HPP
#define MGUARD_FILE_PROCESSOR_HPP

#include "common.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <iostream>
#include <map>

namespace mguard {

using MappingSection = boost::property_tree::ptree;

class FileProcessor
{
public:
  std::vector<std::string>
  readStream(std::string streamPath);

  std::vector<std::string>
  getVectorByDelimiter(std::string _s, std::string delimiter);
};

class AttributeMappingFileProcessor: FileProcessor
{
public:
  AttributeMappingFileProcessor(const std::string& filename);

  std::map<std::string, std::string>& 
  getStreams()
  {
    return m_streams;
  }

  std::map<ndn::Name, mguard::AttributeMappingTable>& 
  getMappingTable()
  {
    return m_mappingTable;
  }

  /*read stream saved in csv file and return data in a vector, per row*/
  std::vector<std::string>
  readStream(std::string streamName);

  bool processAttributeMappingFile();

  bool
  processStreamsSection(const MappingSection &section);

  bool
  processMappingSection(const MappingSection &section);

private:
  const std::string& m_filename;
  std::map<std::string, std::string> m_streams;
  std::map<ndn::Name, mguard::AttributeMappingTable> m_mappingTable;

};

} // namespace mguard

#endif // MGUARD_PRE_PROCESSOR_HPP