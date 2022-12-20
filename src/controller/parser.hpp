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
 
#ifndef MGUARD_PARSER_HPP
#define MGUARD_PARSER_HPP

//#include "common.hpp"
#include "server/util/name-tree.hpp"

#include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/logger.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <iostream>
#include <utility>
#include <ostream>

namespace mguard {
namespace parser {

struct PolicyDetail
{
    std::string policyIdentifier;
    std::list<std::string> streams;
    std::list<std::string> requesters;
    std::string abePolicy;
};

struct ParsedSection
{
    std::list<std::string> allowedNames;
    std::list<std::string> deniedNames;
    std::list<std::pair<std::string, std::string>> allowedTimes;
    std::list<std::pair<std::string, std::string>> deniedTimes;
};

struct SectionDetail
{
    std::list<std::string> streams;
    std::string abePolicy;
};

using ConfigSection = boost::property_tree::ptree;
using NameTree = util::nametree::NameTree;

class PolicyParser
{
public:
  explicit PolicyParser(const std::basic_string<char>& availableStreams);

  void
  parseAvailableStreams(const std::basic_string<char>& streamsFilePath, NameTree& streamsAttributes, NameTree& requesters);

  PolicyDetail
  parsePolicy(const std::basic_string<char>& policyFilePath);


private:
  static std::list<std::string>
  getFilters(ConfigSection &section);

  static std::pair<std::string, std::string>
  parseAttribute(const std::string& attribute);

  static std::list<std::string>
  splitRequesters(const std::string& basicString);

  static std::string 
  doStringThing(const std::list<std::string>& list, const std::string& operation);

  static std::list<std::string> 
  split(const std::string& basicString, const std::string& delimiter);

  static std::string
  processAttributes(const std::list<std::string>& attrList);

  ParsedSection
  parseSection(ConfigSection& section);

  SectionDetail
  calculatePolicy(const ParsedSection& section);

  // information from the available-streams file
  NameTree attStreamsTree, requestersTree;
};

} // namespace parser
} // namespace mguard

#endif // MGUARD_PARSER_HPP