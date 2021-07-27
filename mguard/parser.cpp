#include "parser.hpp"

//#include "dataWindowParameter.cpp"

#include "boost/property_tree/info_parser.hpp"

#include <iostream>

namespace pt = boost::property_tree;

namespace mguard {
    PolicyParser::PolicyParser(std::string &fileName)
    : configFilePath(fileName)
    {}

    bool PolicyParser::processFile() {
        // input for file
        std::ifstream inputFile (configFilePath.c_str());
        if (!inputFile.is_open()){
            std::string msg = "ifstream input failed for " + configFilePath;
            std::cerr << msg << std::endl;
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
            studyID = section.get<std::string>("STUDY-ID");
            dataOwnerID = section.get<std::string>("DATA-OWNER-ID");
            dataRequesterIDs = split(section.get<std::string>("DATA-REQUESTER-IDs"), ",");
            dataStreamName = section.get<std::string>("DATA-STREAM-NAME");
        } catch (const std::exception &exception) {
            // this is usually a syntax error within the policy
            std::cerr << exception.what() << std::endl;
            return false;
        }

        // optional data-window section
        try {
            pt::ptree dw = section.get_child("DATA-WINDOW");
            hasDataWindow = true;
            if (!(
                    processDWSection(dw.get_child("ALLOW"), true) &&
                    processDWSection(dw.get_child("DENY"), false))
                    ) {
                return false;
            }
        } catch (std::exception& e) {
            // just means there is no data-window portion
            hasDataWindow = false;
        }

        return true;
    }

    bool PolicyParser::processDWSection(pt::ptree &section, bool isAllowed) {
        std::string key, value;
        // go through all parameters in the allow/deny section
        for (const auto &parameter : section) {
            // set key, value
            key = parameter.first;
            value = parameter.second.get_value<std::string>();
            // check for valid key
            if (!dataWindowParameter::isValidKey(key)) {
                std::cout << "inValid key " << key << std::endl;
                return false;
            }
            // TODO: change this to make it a section/block like how data-window is
            if (key == "ColumnNameValue"){
                // splits into column name and value
                std::list<std::string> keyValue = split(value, " ");
                parameters.emplace_back(isAllowed, dataStreamName, keyValue.front(), keyValue.back());
            } else {
                // add new parameter object to master list
                parameters.emplace_back(isAllowed, key, value);
            }
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
        return output;
    }

    std::ostream &operator<<(std::ostream &os, const PolicyParser &parser) {
        os << "PolicyParser Object {" << std::endl << " configFilePath:\t" << parser.configFilePath  << std::endl<< " hasDataWindow:\t\t" << parser.hasDataWindow  << std::endl<< " policyID:\t\t"
           << parser.policyID  << std::endl<< " studyID:\t\t" << parser.studyID  << std::endl<< " dataOwnerID:\t\t" << parser.dataOwnerID << std::endl
           << " dataRequesterIDs:\t" ;
        for (const auto &item : parser.dataRequesterIDs) {
            os << item << " ";
        }
        std::cout  << std::endl<< " dataStreamName:\t" << parser.dataStreamName  << std::endl << " parameters {" << std::endl;
        for (const auto &item : parser.parameters) {
           os << "  " << item << std::endl;
        };
        os << " }" << std::endl << "}" << std::endl;
        return os;
    }

    dataWindowParameter::dataWindowParameter(bool isAllowed, std::string key, std::string value)
            :isAllowed(isAllowed)
            ,key(std::move(key))
            ,value(std::move(value))
    {}

    bool dataWindowParameter::isValidKey(const std::string& key) {
        std::list<std::string> allowedKeys = {"StreamName", "ColumnName", "ColumnNameValue"};
        if (std::find(allowedKeys.begin(), allowedKeys.end(), key) == std::end(allowedKeys)){
            return false;
        }
        return true;
    }

    std::ostream &operator<<(std::ostream &os, const dataWindowParameter &parameter) {
        os << "dataWindowParameter Object {" << std::endl << "   isAllowed:\t" << parameter.isAllowed << std::endl << "   key:\t\t" << parameter.key << std::endl << "   value:\t" << parameter.value << std::endl
           << "   streamName:\t" << parameter.streamName << std::endl << "   columnName:\t" << parameter.columnName << std::endl << "  }";
        return os;
    }

    dataWindowParameter::dataWindowParameter(bool isAllowed, std::string streamName, std::string columnName,std::string value)
            :isAllowed(isAllowed)
            ,streamName(std::move(streamName))
            ,columnName(std::move(columnName))
            ,value(std::move(value))
            ,key("columnNameValue")
    {}
}
