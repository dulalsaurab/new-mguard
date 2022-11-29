/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021-2022,  The University of Memphis
 *
 * This file is part of mGuard.
 * See AUTHORS.md for complete list of mGuard authors and contributors.
 *
 * mGuard is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mGuard is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * mGuard, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef MGUARD_FILE_PROCESSOR_HPP
#define MGUARD_FILE_PROCESSOR_HPP

#include "common.hpp"

#include <ndn-cxx/face.hpp>

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
  getVectorByDelimiter(std::string _s, std::string delimiter, int nSize = 1);
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