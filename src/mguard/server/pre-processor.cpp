#include "pre-processor.hpp"
#include <ndn-cxx/util/logger.hpp>

#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <fstream>
#include <iostream>
#include <filesystem>

// NDN_LOG_INIT(mguard.Preprocessor);

namespace mguard
{

DataPreprocessor::DataPreprocessor(std::string mappingFile) 
: m_mappingFilename (mappingFile)
// , m_attributeMappingFileProcessor(mappingFile)
{
  // bool ret = m_attributeMappingFileProcessor.processAttributeMappingFile();
  // if (!ret)
  // {
    // NDN_LOG_INFO ("Attribute mapping file processing failled");
    // exit(-1);
  // }
}


} // mguard