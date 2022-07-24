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

NDN_LOG_INIT(mguard.parser);

namespace pt = boost::property_tree;
namespace mguard {
namespace parser {

  // todo: functionality for wildcard within stream names

PolicyParser::PolicyParser(const std::basic_string<char>& availableStreams)
{
  // store data from input files
  parseAvailableStreams(availableStreams);
}

void
PolicyParser::parseAvailableStreams(const std::basic_string<char>& streamsFilePath)
{
  NDN_LOG_INFO("processing available stream path: " << streamsFilePath);
  // input for available streams
  availableStreams.clear();
  availableStreamLevels.clear();
  allowedRequesters.clear();
  availableAttributes.clear();

  std::ifstream input(streamsFilePath.c_str());
  ConfigSection section;
  boost::property_tree::read_info(input, section);
  // processing given streams and storing all possible streams
  std::list<std::string> levels;
  std::string buildingName;
  for (const auto &item : section.get_child("available-streams"))
  {
    // add stream to list of streams
    NDN_LOG_TRACE("stream name: " << item.first);
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
            throw std::runtime_error("Parsing available-streams failed: " + item.first + " does not start with /");
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

  input.close();
}

PolicyDetail
PolicyParser::parsePolicy(const std::basic_string<char>& policyFilePath) {
    // input for config file
    std::ifstream input(policyFilePath.c_str());
    // parsing of policy

    // loading input file into sections
    ConfigSection fullTree;
    pt::read_info(input, fullTree);
    // set all instance variables (all required in policy)
    std::string policyID = fullTree.get<std::string>("policy-id");
    std::list<std::string> requesterNames = splitRequesters(fullTree.get<std::string>("requester-names"));

    // check given requesters against allowed requesters
    for (const std::string &requester: requesterNames) {
        if (std::find(allowedRequesters.begin(), allowedRequesters.end(), requester) == std::end(allowedRequesters)) {
            // requester is not in allowedRequesters
            throw std::runtime_error("requester " + requester + " not in given requesters");
        }
    }

    std::list<std::string> streams, policies;
    std::cout << "passed filter:" << std::endl;
    for (std::pair<std::string, ConfigSection> primaryTree : fullTree) {
        if (primaryTree.first != "policy-id" && primaryTree.first != "requester-names") {
            SectionDetail parsedAccessControl = parseSection(primaryTree.second);
            for (const std::string &stream: parsedAccessControl.streams) {
                streams.push_back(stream);
            }
            policies.push_back(parsedAccessControl.abePolicy);
        }
    }
    streams.unique();

    std::string policy = doStringThing(policies, "OR");


    input.close();
    return {policyID, streams, requesterNames, policy};
}

SectionDetail
PolicyParser::parseSection(ConfigSection& section) {
    // initialize per-policy variables
    std::list<std::string> calculatedStreams, allowedStreams, allowedAttributes, deniedStreams, deniedAttributes;

    // NOTE: I should figure out better way to structure this part
    // this could possibly be done with section.get_child_optional()
    processAttributeFilter(section.get_child("allow"), allowedStreams, allowedAttributes);
    try {
        processAttributeFilter(section.get_child("deny"), deniedStreams, deniedAttributes);
    }
    catch (boost::wrapexcept<pt::ptree_bad_path> &exception){
    }

    if (allowedStreams.empty()){
        throw std::runtime_error("\"allow\" section needs at least one stream name");
    }

    // start of logic for creating abe policy
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

    // warning for denied stream covering all of an allowed stream
    for (const std::string& warning : allowDenyWarning) {
        NDN_LOG_WARN(warning);
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
    std::string abePolicy = doStringThing(policy, "AND");

    return {calculatedStreams, abePolicy};
}

void
PolicyParser::processAttributeFilter(ConfigSection &section, std::list<std::string> &streams, std::list<std::string> &attributes)
{
    // go through all filters in the allow/deny section
    std::string value;
    for (const auto &parameter : section) {
        value = parameter.first;

        if (std::find(availableStreamLevels.begin(), availableStreamLevels.end(), value) != std::end(availableStreamLevels)) {
            // is a stream name
            streams.push_back(value);
        } else if (std::find(availableAttributes.begin(), availableAttributes.end(), value) != std::end(availableAttributes)) {
            // is an attribute
            attributes.push_back(value);
        } else {
            // isn't stream or attribute
            throw std::runtime_error(value + " not an available stream or attribute");
        }
    }
}

std::string
PolicyParser::processAttributes(const std::list<std::string>& attrList) {
    std::string output, building;
    // go through each one
    // if it's not in the "checked" list, go through the rest of
    std::list<std::string> alreadyCounted;
    for (const std::string& searching : attrList) {
        // if the thing is already counted, skip it
        if (std::find(alreadyCounted.begin(), alreadyCounted.end(), searching) != std::end(alreadyCounted)){
            continue;
        }

        // because of the previous check, the next steps only happen for the amount of types of attributes listed

        // search through attributes for ones of the same type as the current one
        // OR attributes of similar types
        for (const std::string& attr : attrList) {
            if (parseAttribute(searching).first == parseAttribute(attr).first) {
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

std::pair<std::string, std::string>
PolicyParser::parseAttribute(const std::string& attribute)
{
    std::pair<std::string, std::string> out;
    bool foundAttribute = false;
    std::string type, value;
    for (const std::string& checkingItem : split(attribute, "/")) {
        if (!type.empty()) {
            value = checkingItem;
            break;
        }
        if (foundAttribute) {
            type = checkingItem;
            continue;
        }
        if (checkingItem == "ATTRIBUTE") {
            foundAttribute = true;
        }
    }
    out.first = type;
    out.second = value;
    return out;
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
