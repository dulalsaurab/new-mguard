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
            std::string name, buildingName;
            std::list<std::string> levels;
            for (const auto &item : section.get_child("available-streams")) {
                name = item.first;
                levels = split(name, "/");
                buildingName = "";
                for (int index = 0; !levels.empty(); index ++) {
                    // doesn't follow ndn naming convention, but this is how it's currently formatted
                    // current implementation doesn't have a leading '/' for any of the names
                    if (index > 0) {
                        buildingName += "/";
                    }
                    buildingName += levels.front();

                    // only add to list if it's not already there. this prevents duplicates
                    if (std::find(availableStreams.begin(), availableStreams.end(), buildingName) == availableStreams.end()) {
                        // buildingName not in availableStreams
                        availableStreams.push_back(buildingName);
                    }
                    // remove first because it's already been used
                    levels.erase(levels.begin());
                }
            }
            for (const auto &item : section.get_child("users")) {
                // key,value as "'user', buildingName"
                availableUsers.push_back(item.second.get_value<std::string>());
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
            if (!prelimCheck()) {
                // either the requesterIDs or the stremName didn't match
                return false;
            }
        } catch (const std::exception &exception) {
            // this is usually a syntax error within the policy
            std::cerr << exception.what() << std::endl;
            return false;
        }

        // optional attribute-filters section
        // todo: figure out better way to structure this part
        // this could possibly be done with section.get_child_optional()
        try {
            pt::ptree filterTree = section.get_child("attribute-filters");
            hasFilters = true;
            try {
                processAttributeFilter(filterTree.get_child("allow"), true);
            }
            catch (std::exception &e){
                hasAllow = false;
            }
            try {
                processAttributeFilter(filterTree.get_child("deny"), false);
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
        for (const auto &parameter : section) {
            filters.emplace_back(isAllowed, parameter.first);
        }
        return true;
    }


    bool PolicyParser::prelimCheck() {
        // check user
        for (const auto &requester : requesterIDs) {
            if (std::find(availableUsers.begin(), availableUsers.end(), requester) == std::end(availableUsers)) {
                // requester is not in availableUsers
                std::cerr   <<  "user " <<  requester   <<  " not in given users"    << std::endl   ;
                return false;
            }
        }
        // check stream name
        if (std::find(availableStreams.begin(), availableStreams.end(), streamName) == std::end(availableStreams)) {
            std::cerr   <<  streamName  <<  " not in  given stream names"       <<  std::endl   ;
            return false;
        }

        return true;
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
            };
        }
        os      <<
        "\t"    <<  "available_streams info"    <<  std::endl   <<
        "\t\t"  <<  "available-streams"         <<  std::endl   ;
        for (const auto &stream : parser.availableStreams) {
            os  <<
            "\t\t\t"    <<  stream  <<  std::endl;
        }
        os      <<
        "\t\t"  <<  "available-users"           <<  std::endl;
        for (const auto &user : parser.availableUsers) {
            os  <<
            "\t\t\t"    <<  user    <<  std::endl;
        }
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
