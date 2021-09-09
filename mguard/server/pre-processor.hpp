
#ifndef MGUARD_PRE_PROCESSOR_HPP
#define MGUARD_PRE_PROCESSOR_HPP

#include <iostream>
#include <map>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>
#include "common.hpp"
// #include "file-processor.hpp"

namespace mguard
{

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class DataPreprocessor
{

public:
  DataPreprocessor(std::string mappingFile);

  /* 
    get data from cerebral cortex sever and save it in a csv file
    for this we need some sort of API that can be called from here ? 
  */
  bool
  getDataFromCC(std::string streamName, ndn::optional<std::string> query);


private:
  std::string m_mappingFilename;
  // AttributeMappingFileProcessor m_attributeMappingFileProcessor;
  // ndn::Face m_face;
  // friend class AttributeMappingFileProcessor;
};

}

#endif // MGUARD_PRE_PROCESSOR_HPP