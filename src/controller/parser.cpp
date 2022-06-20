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
 
#include "parser.hpp"
#include "boost/property_tree/info_parser.hpp"

#include <iostream>
#include <utility>

//NDN_LOG_INIT(mguard.parser);

namespace pt = boost::property_tree;
namespace mguard {
namespace parser {

/* 
  todo: functionality for wildcard within stream names
  todo: change calling of parser
  1.) initialize by parsing available_streams first
  2.) call parser with policy input and parser will output to file
*/

PolicyParser::PolicyParser(std::basic_string<char> availableStreams)
: availableStreamsPath (std::move(availableStreams))
{
  // store data from input files
  inputStreams();
}

PolicyDetail
PolicyParser::getPolicyInfo() {
//    NDN_LOG_DEBUG("policyID: " << policyID << " abePolicy: " << abePolicy);
    return {policyID, calculatedStreams,requesterNames, abePolicy};
}

bool 
PolicyParser::inputStreams(const std::basic_string<char>& streamsFilePath) {
    // todo: should probably reset everything that would be stored in parser
    // just so that you can't get old data after redoing global variables
//    NDN_LOG_INFO("processing available stream path: " << availableStreamsPath);
    availableStreamsPath = streamsFilePath;
    return inputStreams();
}

bool
PolicyParser::inputStreams() {
    // input for available streams
    std::ifstream availableStreamsFile(availableStreamsPath.c_str());
    // parsing of available streams file
    parseAvailableStreams(availableStreamsFile);
    availableStreamsFile.close();

    return true;
}

bool
PolicyParser::inputPolicy(const std::basic_string<char>& policyFilePath) {
  // input for config file
  std::ifstream policyFile (policyFilePath.c_str());
  // parsing of policy
  parsePolicy(policyFile);
  policyFile.close();
  generateABEPolicy();
  return true;
}

bool 
PolicyParser::parseAvailableStreams(std::istream &input) 
{
  ConfigSection section;
  pt::read_info(input, section);
  // processing given streams and storing all possible streams
  std::list<std::string> levels;
  std::string buildingName;
  for (const auto &item : section.get_child("available-streams"))
  {
    // add stream to list of streams
//    NDN_LOG_TRACE("stream name: " << item.first);
    availableStreams.push_back(item.first);
    // note: possibly not needed since we will have all the internal nodes as attributes. Will probably be listed in the available-streams file
    // adding all parents of given stream to list
    levels = split(item.first, "/");
    std::string adding;
    // all names should start with a /
    buildingName = "/";
    for (int index = 0; !levels.empty(); index ++) {
      adding = levels.front();
      // then remove it from the list we're grabbing from
      levels.erase(levels.begin());
      // first character things
      if (index == 0 ) {
        // checking to make sure it starts with a /
        if (!adding.empty()) {
          return false;
        }
        // don't add the first one
        continue;
      }

      // formatting
      // store first value in the name you're building
      buildingName += adding;
      if (!levels.empty()) {
        // always add a / after each part of the stream if it's not the very last one
        buildingName += "/";
      }

      // adds the first two together instead of one by one
      if (index < 2) {
        continue;
      }

      // only add to list if it's not already there. this prevents duplicates
      if (std::find(availableStreamLevels.begin(), availableStreamLevels.end(), buildingName) != availableStreamLevels.end()) {
        // buildingName in availableStreamLevels
        continue;
      }

      availableStreamLevels.push_back(buildingName);
    }
  }

  // store available requesters
  for (const auto &item : section.get_child("requesters"))
  {
    // key,value as "'user', buildingName"
    allowedRequesters.push_back(item.second.get_value<std::string>());
  }

  // store attributes
  for (const auto &attribute : section.get_child("attributes"))
  {
    availableAttributes.push_back(attribute.first);
  }

  return true;
}

bool 
PolicyParser::parsePolicy(std::istream& input) {
    // loading input file into sections
    ConfigSection section;
    pt::read_info(input, section);
    // set all instance variables (all required in policy)
    policyID = section.get<std::string>("policy-id");
    requesterNames = splitRequesters(section.get<std::string>("requester-names"));

    // check given requesters against allowed requesters
    for (const std::string &requester : requesterNames) {
        if (std::find(allowedRequesters.begin(), allowedRequesters.end(), requester) == std::end(allowedRequesters)) {
            // requester is not in allowedRequesters
            throw std::runtime_error("requester " + requester + " not in given requesters");
        }
    }

    // reset per-policy variables
    calculatedStreams.clear();
    allowedStreams.clear();
    allowedAttributes.clear();
    deniedStreams.clear();
    deniedAttributes.clear();

    // REQUIRED attribute-filters section
    // NOTE: I should figure out better way to structure this part
    // this could possibly be done with section.get_child_optional()
    pt::ptree filterTree = section.get_child("attribute-filters");
    processAttributeFilter(filterTree.get_child("allow"), true);
    try {
        processAttributeFilter(filterTree.get_child("deny"), false);
    }
    catch (std::exception &e) {
    }

    return true;
}

void
PolicyParser::processAttributeFilter(pt::ptree &section, bool isAllowed) 
{
  // go through all filters in the allow/deny section
  std::string value;
  for (const auto &parameter : section) {
      value = parameter.first;

      if (std::find(availableStreamLevels.begin(), availableStreamLevels.end(), value) != std::end(availableStreamLevels)) {
          // is a stream name
          if (isAllowed) {
              allowedStreams.push_back(value);
          } else {
              deniedStreams.push_back(value);
          }
      } else if (std::find(availableAttributes.begin(), availableAttributes.end(), value) != std::end(availableAttributes)) {
          // is an attribute
          if (isAllowed) {
              allowedAttributes.push_back(value);
          } else {
              deniedAttributes.push_back(value);
          }
      } else {
          // isn't stream or attribute
          throw std::runtime_error(value + " not an available stream or attribute");
      }
  }

  if (isAllowed && allowedStreams.empty()){
      throw std::runtime_error("\"allow\" section needs at least one stream name");
  }
}

bool 
PolicyParser::generateABEPolicy() {
    std::list<std::string> policy;

    // stream name processing

    std::list<std::string> workingStreams;

    // warning variables
    std::list<std::string> allowDenyWarning;
    std::string warn;
    // add everything under all allowed stream names
    for (const std::string &available: availableStreams) {
        for (const std::string &allowed: allowedStreams) {
            // if it's allowed and not a duplicate, add it to the list

            // if the available stream is a child of the allowed stream, that available stream should be allowed
            if ((available.rfind(allowed, 0) == 0) &&
                // if the available stream isn't already in workingStreams
                (std::find(workingStreams.begin(), workingStreams.end(), available) ==
                  std::end(workingStreams))) {
                bool add = true;
                // for each allowed stream, check against the denied streams
                for (const std::string &denied: deniedStreams) {
                    // add to warning if allowed stream is the stream or child of denied stream
                    if (allowed.rfind(denied, 0) == 0) {
                        warn = "WARNING: " + allowed + " is the same stream or a child of the denied stream " + denied;
                        if (std::find(allowDenyWarning.begin(), allowDenyWarning.end(), warn) == std::end(allowDenyWarning)) {
                            allowDenyWarning.push_back(warn);
                        }
                    }
                    // checks if available stream is a child of any of the denied streams
                    // if it is, don't add it to the allowed streams list
                    if (available.rfind(denied, 0) == 0) {
                        add = false;
                        break;
                    }
                }

                // if it passed all checks against the denied streams, add it to the list
                if (add) {
                    workingStreams.push_back(available);
                }
            }
        }
    }

    // todo: see how to do this with ndn logs
    // warning for denied stream covering all of an allowed stream
    for (const std::string& warning : allowDenyWarning) {
//        NDN_LOG_WARN(warning);
    }

    // error for if no streams are allowed
    if (workingStreams.empty()) {
        throw std::runtime_error("No streams allowed by policy");
    }

    calculatedStreams = workingStreams;
    policy.emplace_back(doStringThing(workingStreams, "OR"));

    // attribute processing
    // AND allow attributes
    if (!allowedAttributes.empty()) {
        policy.emplace_back(processAttributes(allowedAttributes));
    }

    // OR deny attributes
    if (!deniedAttributes.empty()) {
        std::list<std::string> workingAttributes = availableAttributes;
        for (std::string &toRemove : deniedAttributes) {
            workingAttributes.remove(toRemove);
        }
        if (!workingAttributes.empty()) {
            policy.emplace_back(doStringThing(workingAttributes, "OR"));
        } else {
            // all attributes are denied
            policy.clear();
            throw std::runtime_error("Cannot deny all attributes");
        }
    }

    // putting it all all together
    // AND together all separate conditions made for the output policy
    abePolicy = doStringThing(policy, "AND");

    // output
    std::fstream file;
    file.open("parser_output", std::ios::app);
    for (const auto &requester: requesterNames) {
        file << requester << '\t' << abePolicy << std::endl;
    }
    file.close();

    return true;
}

std::string 
PolicyParser::processAttributes(const std::list<std::string>& attrList) {
    std::string output, building;
    // go through each one
    // if it's not in the "checked" list, go through the rest of
    std::list<std::string> alreadyCounted;
    for (std::string searching : attrList) {
        // if the thing is already counted, skip it
        if (std::find(alreadyCounted.begin(), alreadyCounted.end(), searching) != std::end(alreadyCounted)){
            continue;
        }

        // because of the previous check, the next steps only happen for the amount of types of attributes listed

        // search through attributes for ones of the same type as the current one
        // OR attributes of similar types
        for (std::string attr : attrList) {
            if (isAlike(searching, attr)) {
                if (!building.empty()) {
                    building += " OR ";
                }
                building += attr;
                alreadyCounted.push_back(attr);
            }
        }

        // AND together sets of similar attributes
        if (!building.empty()) {
            building = "(" + building + ")";
            if (!output.empty()) {
                output += " AND ";
            }
            output += building;
            building = "";
        }
    }

    return output;
}

bool 
PolicyParser::isAlike(std::string &attribute, std::string &checking) 
{
    // assumes /attribute/type/value format for attributes

    // split the inputs into the different levels
    std::list<std::string> given = split(attribute, "/");
    std::list<std::string> testing = split(checking, "/");

    // will store the attribute types in these variables
    std::string givenType;
    std::string checkingType;

    // once you find "attribute", store the next value and stop
    bool foundAttribute = false;
    for (const std::string& item : given) {
        if (foundAttribute) {
            givenType = item;
            break;
        }
        if (item == "attribute") {
            foundAttribute = true;
        }
    }

    foundAttribute = false;
    for (const std::string& checkingItem : testing) {
        if (foundAttribute) {
            checkingType = checkingItem;
            break;
        }
        if (checkingItem == "attribute") {
            foundAttribute = true;
        }
    }

    // if the values are the same, the types of attributes are also the same
    return givenType == checkingType;
}

std::string 
PolicyParser::doStringThing(const std::list<std::string> &list, const std::string& operation) 
{
    if (list.size() == 1) {
        return list.front();
    }
    std::string out;
    for (const std::string &thing : list) {
        if (out.empty()) {
            out += "(";
        } else {
            out += " " + operation + " ";
        }
        out += thing;
    }
    out += ")";
    return out;
}

// splitting string into list of strings along delimeter
std::list<std::string> 
PolicyParser::split(const std::string &basicString, const std::string &delimeter) 
{
    std::list<std::string> splittedString;
    std::size_t startIndex = 0;
    std::size_t endIndex;
    std::string val;
    while ((endIndex = basicString.find(delimeter, startIndex)) < basicString.size()){
        val = basicString.substr(startIndex, endIndex - startIndex);
        splittedString.push_back(val);
        startIndex = endIndex + delimeter.size();
    }
    if  (startIndex < basicString.size()) {
        val = basicString.substr(startIndex);
        splittedString.push_back(val);
    }
    return splittedString;
}

std::list<std::string> 
PolicyParser::splitRequesters(const std::string& basicString) 
{
    std::list<std::string> output;
    std::list<std::string> n = PolicyParser::split(basicString, ",");
    for (const auto& item : n) {
        for (const auto& thing : PolicyParser::split(item, " ")) {
            if (!thing.empty()) {
                output.push_back(thing);
            }
        }
    }
    return output;
}
} // namespace parser
} // namespace mguard
