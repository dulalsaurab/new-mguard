#include "parser.hpp"

//#include "attributeFilter.cpp"

#include "boost/property_tree/info_parser.hpp"

#include <iostream>
#include <utility>

namespace pt = boost::property_tree;

namespace mguard {

    PolicyParser::PolicyParser(std::string &configFilePath, std::string &availableStreams)
    : configFilePath      (std::move(configFilePath))
    , availableStreamsPath    (std::move(availableStreams))
    {}

    bool PolicyParser::processFiles() {
        // todo:    move processing of available_streams to separate method to only be called once
        // todo:    the processPolicy() method should be called separately from main
        // input for available streams

        std::ifstream availableStreamsFile(availableStreamsPath.c_str());
        if (!availableStreamsFile.is_open()){
            std::cerr   <<  "ifstream input failed for "    <<  availableStreamsPath    <<  std::endl;
            return false;
        }
        // parsing of available streams file
        if (!parseAvailableStreams(availableStreamsFile)){
            std::cerr   <<  "parsing of available-streams failed"   <<  std::endl;
            return false;
        }
        availableStreamsFile.close();

        // input for file
        std::ifstream policyFile (configFilePath.c_str());
        if (!policyFile.is_open()){
            std::cerr   <<  "ifstream input failed for "    <<  configFilePath          <<  std::endl;
            return false;
        }
        // parsing of policy
        if (!parsePolicy(policyFile)) {
            std::cerr   <<  "parsing of policy failed"      <<  std::endl;
            return false;
        }
        policyFile.close();

        // process policy data
        generateABEPolicy();

        return true;
    }

    bool PolicyParser::parseAvailableStreams(std::istream &input) {
        ConfigSection section;

        try {
            pt::read_info(input, section);
        } catch (pt::info_parser_error& error) {
            std::cerr   <<  "read_info failed for available-streams"  <<  std::endl;
            return false;
        }

        try {
            // processing given streams and storing all possible streams
            std::list<std::string> levels;
            std::string buildingName;
            for (const auto &item : section.get_child("available-streams")) {
                availableStreams.push_back(item.first);
                levels = split(item.first, "/");
                buildingName = "";
                for (int index = 0; !levels.empty(); index ++) {
                    // doesn't follow ndn naming convention, but this is how it's currently formatted
                    // current implementation doesn't have a leading '/' for any of the names
                    if (index > 0) {
                        buildingName += "/";
                    }
                    buildingName += levels.front();
                    // only add to list if it's not already there. this prevents duplicates
                    if (std::find(availableStreamLevels.begin(), availableStreamLevels.end(), buildingName) == availableStreamLevels.end()) {
                        // buildingName not in availableStreamLevels
                        availableStreamLevels.push_back(buildingName);
                    }
                    // remove first because it's already been used
                    levels.erase(levels.begin());
                }
            }
            // set prefix to the first value in calculated availableStreamLevels
            prefix = availableStreamLevels.front();

            // store available requesters
            for (const auto &item : section.get_child("requesters")) {
                // key,value as "'user', buildingName"
                allowedRequesters.push_back(item.second.get_value<std::string>());
            }

            // store attributes
            for (const auto &attribute : section.get_child("attributes")) {
                availableAttributes.push_back(attribute.first);
            }

        } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &error) {
            std::cerr   <<  error.what()    <<  std::endl;
            return false;
        }

        return true;
    }

    bool PolicyParser::parsePolicy(std::istream& input) {
        // loading input file into sections
        ConfigSection section;
        try {
            pt::read_info(input, section);
        } catch (pt::info_parser_error& error) {
            std::cerr << "read_info failed" << std::endl;
            return false;
        }
        try {
            // set all instance variables (all required in policy)
            policyID = section.get<int>("policy-id");
            requesterIDs = split(section.get<std::string>("requester-ids"), ",");
            streamName = section.get<std::string>("stream-name");
        } catch (const std::exception &exception) {
            // this is usually a syntax error within the policy
            std::cerr << exception.what() << std::endl;
            return false;
        }

        // check requester against allowed requesters
        for (const std::string &requester : requesterIDs) {
            if (std::find(allowedRequesters.begin(), allowedRequesters.end(), requester) == std::end(allowedRequesters)) {
                // requester is not in allowedRequesters
                std::cerr   <<  "requester " <<  requester   <<  " not in given requesters"    << std::endl   ;
                return false;
            }
        }
        // check stream name against available streams
        if (std::find(availableStreamLevels.begin(), availableStreamLevels.end(), streamName) == std::end(availableStreamLevels)) {
            std::cerr   <<  streamName  <<  " not in  given stream names"       <<  std::endl   ;
            return false;
        }

        // optional attribute-filters section
        // todo: figure out better way to structure this part
        // this could possibly be done with section.get_child_optional()
        try {
            pt::ptree filterTree = section.get_child("attribute-filters");
            hasFilters = true;
            try {
                if (!processAttributeFilter(filterTree.get_child("allow"), true)) {
                    return false;
                }
            }
            catch (std::exception &e){
                hasAllow = false;
            }
            try {
                if (!processAttributeFilter(filterTree.get_child("deny"), false)) {
                    return false;
                }
            }
            catch (std::exception &e) {
                hasDeny = false;
            }
        }
        catch (std::exception& e) {
            // just means there is no attribute-filter section
            hasFilters = false;
        }
        return true;
    }

    bool PolicyParser::processAttributeFilter(pt::ptree &section, bool isAllowed) {
        // go through all filters in the allow/deny section
        std::string value;
        for (const auto &parameter : section) {
            value = parameter.first;
            filters.emplace_back(isAllowed, value);

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
                std::cerr   <<  value   <<  " not an available stream or attribute" <<  std::endl;
                return false;
            }
        }
        return true;
    }

    bool PolicyParser::generateABEPolicy() {
        if (hasFilters) {
            std::list<std::string> policy;
            // stream name processing
            std::list<std::string> workingStreams;
            if (allowedStreams.empty()) {
                allowedStreams.push_back(streamName);
            }
            // add everything under all allowed stream names
            for (const std::string &available : availableStreams) {
                for (const std::string &allowed : allowedStreams) {
                    // if it's allowed and not a duplicate, add it to the list
                    if ((available.rfind(allowed, 0) == 0) && (std::find(workingStreams.begin(), workingStreams.end(), available) == std::end(workingStreams))) {
                        bool add = true;
                        for (const std::string &denied : deniedStreams) {
                            if (available.rfind(denied, 0) == 0) {
                                add = false;
                                break;
                            }
                        }
                        if (add) {
                            workingStreams.push_back(available);
                        }
                    }
                }
            }

            policy.emplace_back(doStringThing(workingStreams, "OR"));

            // attribute processing
            // AND allow attributes
            if (!allowedAttributes.empty()) {
                policy.emplace_back(processAttributes(allowedAttributes));
//                policy.emplace_back(doStringThing(allowedAttributes, "AND"));
            }

            // OR deny attributes
            // todo: fix edge case where every attribute is denied which would cause no attributes to be denied in the ABE policy
            if (!deniedAttributes.empty()) {
                std::list<std::string> workingAttribute = availableAttributes;
                for (std::string &toRemove : deniedAttributes) {
                    workingAttribute.remove(toRemove);
                }
                if (!workingAttribute.empty()) {
                    policy.emplace_back(doStringThing(workingAttribute, "OR"));
                }
            }

            // putting all together
            abePolicy = doStringThing(policy, "AND");

        } else {
            std::string streams;
            // no filters means it just gets everything under the stream name
            if (streamName == prefix) {
                abePolicy = "(" + streamName + ")";
            } else {
                for (const std::string &stream : availableStreams) {
                    if (stream.rfind(streamName, 0) == 0) {
                        if (!streams.empty()) {
                            streams += " OR ";
                        } else {
                            streams += "(";
                        }
                        streams += stream;
                    }
                }
                streams += ")";
                abePolicy = streams;
            }
        }
        return true;
    }

    std::string PolicyParser::processAttributes(const std::list<std::string>& attrList) {
        std::string output, building;
        // go through each one
        // if it's not in the "checked" list, go through the rest of
        std::list<std::string> alreadyCounted;
        for (std::string searching : attrList) {
            // if the thing is already counted, skip it
            if (std::find(alreadyCounted.begin(), alreadyCounted.end(), searching) != std::end(alreadyCounted)){
                continue;
            }
            for (std::string attr : attrList) {
                if (isAlike(searching, attr)) {
                    if (!building.empty()) {
                        building += " OR ";
                    }
                    building += attr;
                    alreadyCounted.push_back(attr);
                }
            }
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

    bool PolicyParser::isAlike(std::string &attribute, std::string &checking) {
        std::list<std::string> first = split(attribute, "/"), second = split(checking, "/");

        // scuffed, but should work
        // attributes currently follow "attribute/type/name"
        // if the first two are the same, they're of the same type

        int matches = 0;
        std::string workingFirst, workingSecond;
        first.remove(first.front());
        second.remove(second.front());
        for (int index = 1; index < 3; index++) {
            if (first.front() == second.front()) {
                matches ++;
            }
            first.remove(first.front());
            second.remove(second.front());
        }

        // if matches == 2, the first two parts of the attributes are the same
        return matches == 2;
    }

    std::string PolicyParser::doStringThing(const std::list<std::string> &list, const std::string& operation) {
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
    std::list<std::string> PolicyParser::split(const std::string &basicString, const std::string &delimiter) {
        std::list<std::string> output;
        for (int end, start = 0; start < basicString.size() - 1; start = end + delimiter.size()){
            end = (int)basicString.find(delimiter, start);
            if (end == -1){ // this means it hit the end, so it just adds the rest of the string
                end = (int)basicString.size();
            }
            output.push_back(basicString.substr(start, end - start));
        }

        // edge case where the loop doesn't run
        if (basicString.size() == 1) {
            output.push_back(basicString);
        }

        for (const auto &item : output) {
            // todo: strip the item of leading and trailing spaces
        }

        return output;
    }

    // printing for PolicyParser object
    std::ostream &operator<<(std::ostream &os, const PolicyParser &parser) {
        os <<
        "PolicyParser Object "      <<  std::endl   <<
        "\t"    <<  "policy info"   <<  std::endl   <<
//        "\t\t"  <<  "configFilePath"    <<  "\t\t"  <<  parser.configFilePath   <<  std::endl   <<
//        "\t\t"  <<  "hasFilters"        <<  "\t\t"  <<  parser.hasFilters       <<  std::endl   <<
        "\t\t"  <<  "policyID"          <<  "\t\t"  <<  parser.policyID         <<  std::endl   <<
        "\t\t"  <<  "requesterIDs"      <<  "\t\t"  ;
        for (const auto &item : parser.requesterIDs) {
            os  <<  item        <<  " " ;
        }
        os      <<  std::endl   <<
        "\t\t"  <<  "streamName"        <<  "\t\t"  <<  parser.streamName       <<  std::endl   ;
        if (parser.hasFilters){
            os  <<
            "\t\t"  <<  "filters"   <<  std::endl   ;
            for (const auto &item : parser.filters) {
                os  <<  item    ;
            }
        }
        os      <<
        "\t"    <<  "available_streams info"    <<  std::endl   <<
        "\t\t"  <<  "available-streams"         <<  std::endl   ;
        for (const auto &stream : parser.availableStreamLevels) {
            os  <<
            "\t\t\t"    <<  stream  <<  std::endl;
        }
        os      <<
        "\t\t"  <<  "available-users"           <<  std::endl;
        for (const auto &user : parser.allowedRequesters) {
            os  <<
            "\t\t\t"    <<  user    <<  std::endl;
        }

        os      <<
        "\t"    <<  "ABE info"      <<  std::endl       <<
        "\t\t"  <<  "prefix"        <<  "\t\t"          <<  parser.prefix       <<  std::endl   <<
        "\t\t"  <<  "abe policy"    <<  "\t"            <<  parser.abePolicy    <<  std::endl   ;
        return os;
    }


    attributeFilter::attributeFilter(bool isAllowed, std::string attribute)
            :isAllowed(isAllowed)
            ,attribute(std::move(attribute))
    {}

    // printing for attribute filter
    std::ostream &operator<<(std::ostream &os, const attributeFilter &filter) {
        os  <<  "\t\t\t"    ;
        if (filter.isAllowed) {
            os  <<  "allow" ;
        } else {
            os  <<  "deny"  ;
        }
        os  <<
        "\t"    <<  filter.attribute    <<  std::endl   ;

        return os;
    }
}
