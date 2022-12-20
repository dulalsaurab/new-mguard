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
    parseAvailableStreams(availableStreams, attStreamsTree, requestersTree);
}

void
PolicyParser::parseAvailableStreams(const std::basic_string<char>& streamsFilePath, NameTree& streamsAttributes, NameTree& requestors)
{
    NDN_LOG_INFO("processing available stream path: " << streamsFilePath);
    // input for available streams
    std::ifstream input(streamsFilePath.c_str());
    ConfigSection section;
    boost::property_tree::read_info(input, section);
    // processing given streams and storing all possible streams

    for (const auto &item: section.get_child("available-streams")) {
        // add stream to list of streams
        NDN_LOG_TRACE("stream name: " << item.first);
        streamsAttributes.insertName(item.first);
    }
    for (const auto &item : section.get_child("attributes")) {
        // add stream to list of streams
        NDN_LOG_TRACE("attribute name: " << item.first);
        streamsAttributes.insertName(item.first);
    }

    // store available requesters
    for (const auto &item : section.get_child("requesters")) {
        // key,value as "'user', buildingName"
        requestors.insertName(item.second.get_value<std::string>());
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
        auto a = requestersTree.getLeaves("", {});
        if (std::find(a.begin(), a.end(), (ndn::Name)requester) == a.end()) {
            // requester is not in allowedRequesters
            throw std::runtime_error("requester " + requester + " not in given requesters");
        }
    }

    std::list<std::string> streams, policies;
    for (std::pair<std::string, ConfigSection> primaryTree : fullTree) {
        if (primaryTree.first != "policy-id" && primaryTree.first != "requester-names") {
            auto a = parseSection(primaryTree.second);
            SectionDetail parsedAccessControl = calculatePolicy(a);
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

ParsedSection
PolicyParser::parseSection(ConfigSection& section) {
    // initialize per-policy variables
    std::list<std::pair<std::string, std::string>> allow, deny;

    // NOTE: I should figure out better way to structure this part
    // this could possibly be done with section.get_child_optional()
    auto allowField = section.get_child("allow");
    for (const auto& item : allowField) {
        std::pair<std::string, std::string> tmp;
        tmp.first = item.first;
        tmp.second = item.second.data();
        allow.push_back(tmp);
    }

    auto denyField = section.get_child_optional("deny");
    if (denyField.has_value()) {
        for (const auto& item : denyField.value()) {
            std::pair<std::string, std::string> tmp;
            tmp.first = item.first;
            tmp.second = item.second.data();
            deny.push_back(tmp);
        }
    }

    std::list<std::string> timeKeywords = {"on", "at", "before", "before-include", "after", "after-include", "from", "to"};
    ParsedSection tmp;
    for (const auto &filter: allow) {
        if (std::find(timeKeywords.begin(), timeKeywords.end(), filter.first) != std::end(timeKeywords)) {
            tmp.allowedTimes.push_back(filter);
        } else {
            tmp.allowedNames.push_back(filter.first);
        }
    }
    for (const auto &filter: deny) {
        if (std::find(timeKeywords.begin(), timeKeywords.end(), filter.first) != std::end(timeKeywords)) {
            tmp.deniedTimes.push_back(filter);
        } else {
            tmp.deniedNames.push_back(filter.first);
        }
    }
    if (tmp.allowedNames.empty()) {
        throw std::runtime_error("\"allow\" section needs at least one valid name");
    }
    return tmp;
}

SectionDetail
PolicyParser::calculatePolicy(const ParsedSection& section){

    // start of logic for creating abe policy
    std::list<std::string> policy;

    ndn::Name attrName = attStreamsTree.findNode("ATTRIBUTE")->m_fullName;

    NameTree allowedTree;

    for (const auto &a : section.allowedNames) {
        for (const auto &leaf: attStreamsTree.getLeaves(a, {attrName})) {
            allowedTree.insertName(leaf);
        }
    }
    NameTree allowedAttributes, allowedStreams;


    for (const auto &a : allowedTree.getLeaves(attrName, {})) {
        allowedAttributes.insertName(a);
    }
    for (const auto &a : allowedTree.getLeaves("", {attrName})) {
        allowedStreams.insertName(a);
    }

    NameTree deniedTree;
    for (const auto &a : section.deniedNames) {
        deniedTree.insertName(a);
    }
    NameTree deniedAttributes, deniedStreams;

    for (const auto &a : deniedTree.getLeaves(attrName, {})) {
        deniedAttributes.insertName(a);
    }
    for (const auto &a : deniedTree.getLeaves("", {attrName})) {
        deniedStreams.insertName(a);
    }

    std::list<std::string> workingStreams;
    // get allowed streams while ignoring the denied ones
    for (auto &a:  allowedStreams.getLeaves("", deniedStreams.getLeaves("", {}))) {
        workingStreams.push_back(a.toUri());
    }

    // error for if no streams are allowed
    if (workingStreams.empty()) {
        throw std::runtime_error("No streams allowed by policy");
    }

    policy.emplace_back(doStringThing(workingStreams, "OR"));

    // attribute processing
    auto root = allowedAttributes.findNode("ATTRIBUTE");
    if (root != nullptr) {
        for (const auto &type: root->m_children) {
            std::list<std::string> tmp;
            for (const auto &value: type->m_children) {
                tmp.push_back(value->m_fullName.toUri());
            }
            if (!tmp.empty()) {
                policy.emplace_back(doStringThing(tmp, "OR"));
            }
        }
    }

    root = deniedAttributes.findNode("ATTRIBUTE");
    if (root != nullptr) {
        for (const auto &type : root->m_children) {
            std::vector<ndn::Name> denied;
            for (const auto &value: type->m_children) {
                denied.push_back(value->m_fullName);
            }
            auto tmpMaster = attStreamsTree.getLeaves(type->m_fullName.toUri(), denied);
            std::list<std::string> tmp;
            for (const auto &item: tmpMaster) {
                tmp.push_back(item.toUri());
            }
            if (!tmp.empty()) {
                policy.emplace_back(doStringThing(tmp, "OR"));
            }
        }
    }

    for (const auto &allowedTime: section.allowedTimes) {
        try {
            std::stoi(allowedTime.second);
        } catch (std::invalid_argument) {
            throw std::runtime_error("not able to convert \"" + allowedTime.second + "\" into an int");
        }
        if (allowedTime.second.size() != 10) {
            throw std::runtime_error("UNIX timestamps are 10 digits. " + allowedTime.second + " is not 10 digits.");
        }

        if (allowedTime.first == "on") {
            // policy.push_back("timestamp " + allowedTime.second);
            // this is a date lookup (inherently semantic)
        } else if (allowedTime.first == "at") {
            policy.push_back("time = " + allowedTime.second);

        } else if (allowedTime.first == "before") {
            policy.push_back("time < " + allowedTime.second);

        } else if (allowedTime.first == "before-include") {
            policy.push_back("time <= " + allowedTime.second);

        } else if (allowedTime.first == "after") {
            policy.push_back("time > " + allowedTime.second);

        } else if (allowedTime.first == "after-include") {
            policy.push_back("time >= " + allowedTime.second);

        } else if (allowedTime.first == "from") {
            policy.push_back("time >= " + allowedTime.second);

        } else if (allowedTime.first == "to") {
            policy.push_back("time < " + allowedTime.second);

        } else if (allowedTime.first == "to-include") {
            policy.push_back("time <= " + allowedTime.second);

        } else {
            throw std::runtime_error("Something is wrong with the keyword of " + allowedTime.first );

        }
    }

    for (const auto &deniedTime: section.deniedTimes) {
        try {
            std::stoi(deniedTime.second);
        } catch (std::invalid_argument) {
            throw std::runtime_error("not able to convert \"" + deniedTime.second + "\" into an int");
        }
        if (deniedTime.second.size() != 10) {
            throw std::runtime_error("UNIX timestamps are 10 digits. " + deniedTime.second + " is not 10 digits.");
        }

        if (deniedTime.first == "on") {
            // policy.push_back("timestamp " + deniedTime.second);
            // this is a date lookup (inherently semantic)
        } else if (deniedTime.first == "at") {
            policy.push_back("time > " + deniedTime.second);
            policy.push_back("time < " + deniedTime.second);

        } else if (deniedTime.first == "before") {
            policy.push_back("time >= " + deniedTime.second);

        } else if (deniedTime.first == "before-include") {
            policy.push_back("time > " + deniedTime.second);

        } else if (deniedTime.first == "after") {
            policy.push_back("time <= " + deniedTime.second);

        } else if (deniedTime.first == "after-include") {
            policy.push_back("time < " + deniedTime.second);

        } else if (deniedTime.first == "from") {
            policy.push_back("time < " + deniedTime.second);

        } else if (deniedTime.first == "to") {
            policy.push_back("time >= " + deniedTime.second);

        } else if (deniedTime.first == "to-include") {
            policy.push_back("time > " + deniedTime.second);

        } else {
            throw std::runtime_error("Something is wrong with the keyword of " + deniedTime.first );

        }
    }
    // putting it all together
    // AND together all separate conditions made for the output policy
    std::string abePolicy = doStringThing(policy, "AND");

    return {workingStreams, abePolicy};
}

std::list<std::string>
PolicyParser::getFilters(ConfigSection &section) {
    std::list<std::string> filters;
    for (const auto &parameter : section) {
        filters.push_back(parameter.first);
    }
    return filters;
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

        // getNode through attributes for ones of the same type as the current one
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

// splitting string into list of strings along delimiter
std::list<std::string> 
PolicyParser::split(const std::string &basicString, const std::string &delimiter)
{
    std::list<std::string> splitString;
    std::size_t startIndex = 0;
    std::size_t endIndex;
    std::string val;
    while ((endIndex = basicString.find(delimiter, startIndex)) < basicString.size()){
        val = basicString.substr(startIndex, endIndex - startIndex);
        splitString.push_back(val);
        startIndex = endIndex + delimiter.size();
    }
    if  (startIndex < basicString.size()) {
        val = basicString.substr(startIndex);
        splitString.push_back(val);
    }
    return splitString;
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
