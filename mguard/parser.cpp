#include "parser.hpp"

//#include "attributeFilter.cpp"

#include "boost/property_tree/info_parser.hpp"

#include <iostream>
#include <utility>

namespace pt = boost::property_tree;

namespace mguard {
    PolicyParser::PolicyParser(std::string &fileName)
    : configFilePath(fileName)
    {}

    bool PolicyParser::processFile() {
        // input for file
        std::ifstream inputFile (configFilePath.c_str());
        if (!inputFile.is_open()){
            std::cerr   <<  "ifstream input failed for "    <<  configFilePath  <<  std::endl;
            return false;
        }

        bool out = loadAndParse(inputFile);
        inputFile.close();

        return out;
    }
    bool PolicyParser::loadAndParse(std::istream& input) {
        // loading input file into sections
        ConfigSection section;
        try{
            pt::read_info(input, section);
        } catch (pt::info_parser_error& error) {
            std::cerr << "read_info failed" << std::endl;
            return false;
        }
        try {
            // set all instance variables (all required in policy)
            policyID = section.get<int>("POLICY-ID");
            dataRequesterIDs = split(section.get<std::string>("DATA-REQUESTER-IDs"), ",");
            dataStreamName = section.get<std::string>("DATA-STREAM-NAME");
        } catch (const std::exception &exception) {
            // this is usually a syntax error within the policy
            std::cerr << exception.what() << std::endl;
            return false;
        }

        // optional attribute-filters section
        // todo: figure out better way to structure this part
        try {
            pt::ptree filterTree = section.get_child("ATTRIBUTE-FILTERS");
            hasFilters = true;
            try {
                processAttributeFilter(filterTree.get_child("ALLOW"), true);
            }
            catch (std::exception &e){
                hasAllow = false;
            }
            try {
                processAttributeFilter(filterTree.get_child("DENY"), false);
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
        for (const auto &item : output) {
            // todo: strip the item of leading and trailing spaces
        }
        return output;
    }

    // printing for PolicyParser object
    std::ostream &operator<<(std::ostream &os, const PolicyParser &parser) {
        os <<
        "PolicyParser Object {" <<  std::endl   <<
        "\t"    <<  "configFilePath"    <<  "\t\t"  <<  parser.configFilePath   <<  std::endl   <<
        "\t"    <<  "hasFilters"        <<  "\t\t"  <<  parser.hasFilters       <<  std::endl   <<
        "\t"    <<  "policyID"          <<  "\t\t"  <<  parser.policyID         <<  std::endl   <<
        "\t"    <<  "dataRequesterIDs"  <<  "\t" ;

        for (const auto &item : parser.dataRequesterIDs) {
            os << item << " ";
        }

        os      <<  std::endl           <<
        "\t"    <<  "dataStreamName"    <<  "\t\t"  <<  parser.dataStreamName   <<  std::endl   <<
        "\t"    <<  "filters {"         <<  std::endl;

        for (const auto &item : parser.filters) {
           os << item;
        };

        os  <<
        "\t"    <<  "}"     <<  std::endl <<
        "}"                 <<  std::endl;

        return os;
    }

    attributeFilter::attributeFilter(bool isAllowed, std::string attribute)
            :isAllowed(isAllowed)
            ,attribute(std::move(attribute))
    {}

    bool PolicyParser::isValidKey(const std::string& key) {
        std::list<std::string> allowedKeys = {"policy-id", "data-requester-IDs", "data-stream-name"};
        if (std::find(allowedKeys.begin(), allowedKeys.end(), key) == std::end(allowedKeys)){
            return false;
        }
        return true;
    }

    // printing for attribute filter
    std::ostream &operator<<(std::ostream &os, const attributeFilter &filter) {
        os <<
        "\t\t"      <<  "attributeFilter Object {"                      <<  std::endl   <<
        "\t\t\t"    <<  "isAllowed:"    <<  "\t"    << filter.isAllowed <<  std::endl   <<
        "\t\t\t"    <<  "attribute:"    <<  "\t"    << filter.attribute <<  std::endl   <<
        "\t\t"      <<  "}"                                             <<  std::endl;
        return os;
    }
}
