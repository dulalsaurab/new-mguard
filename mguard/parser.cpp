#include "parser.hpp"
#include "boost/property_tree/info_parser.hpp"

#include <iostream>

namespace pt = boost::property_tree;
namespace mguard {

// todo: remove root func
    PolicyParser::PolicyParser(std::string &configFilePath, std::string &availableStreams)
    : configFilePath        (std::move(configFilePath))
    , availableStreamsPath  (std::move(availableStreams))
    {
        // store data from input files
        bool successfulParse = parseFiles();

        // if parsing went well, process stored data to create ABE policy
        if (successfulParse) {
            generateABEPolicy();
            // todo: make it output this to a file
            //        "name1, name2"
            //        output: (could be output file)
            //        (name1, abeExpression),
            //                (name2, abeExpression)
            std::fstream file;
            file.open("parser_output", std::ios::app);
            for (const auto &requester: requesterNames) {
                file << requester << '\t' << abePolicy << std::endl;
            }
            file.close();
        }
    }

    bool PolicyParser::parseFiles() {

        // input for available streams
        std::ifstream availableStreamsFile(availableStreamsPath.c_str());
        if (!availableStreamsFile.is_open()){
            std::cerr   <<  "ifstream input failed for "    <<  availableStreamsPath    <<  std::endl;
            availableStreamsFile.close();
            return false;
        }
        // parsing of available streams file
        if (!parseAvailableStreams(availableStreamsFile)){
            std::cerr   <<  "parsing of available-streams failed"   <<  std::endl;
            availableStreamsFile.close();
            return false;
        }
        availableStreamsFile.close();

        // input for config file
        std::ifstream policyFile (configFilePath.c_str());
        if (!policyFile.is_open()){
            std::cerr   <<  "ifstream input failed for "    <<  configFilePath          <<  std::endl;
            policyFile.close();
            return false;
        }
        // parsing of policy
        if (!parsePolicy(policyFile)) {
            std::cerr   <<  "parsing of policy failed"      <<  std::endl;
            policyFile.close();
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
            // processing given streams and storing all possible streams
            std::list<std::string> levels;
            std::string buildingName;
            for (const auto &item : section.get_child("available-streams")) {
                // add stream to list of streams
                availableStreams.push_back(item.first);
                // todo: possibly not needed since we will have all the internal nodes as attributes. Will probably be listed in the available-streams file
                // adding all parents of given stream to list
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
                    // remove first because it's already been used in buildingName
                    levels.erase(levels.begin());
                }
            }
            // todo: probably don't need this with new changes
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
            // todo: requesterNames split isn't working right
            auto raw = section.get<std::string>("requester-names");
            auto splitted = split(raw, ",");
            requesterNames = split(section.get<std::string>("requester-names"), ",");
        } catch (const std::exception &exception) {
            // this is usually a syntax error within the policy
            std::cerr << exception.what() << std::endl;
            return false;
        }

        // check given requesters against allowed requesters
        for (const std::string &requester : requesterNames) {
            if (std::find(allowedRequesters.begin(), allowedRequesters.end(), requester) == std::end(allowedRequesters)) {
                // requester is not in allowedRequesters
                std::cerr   <<  "requester " <<  requester   <<  " not in given requesters"    << std::endl   ;
                return false;
            }
        }

        // REQUIRED attribute-filters section
        // todo: figure out better way to structure this part
        // this could possibly be done with section.get_child_optional()
        try {
            pt::ptree filterTree = section.get_child("attribute-filters");
            try {
                if (!processAttributeFilter(filterTree.get_child("allow"), true)) {
                    return false;
                }
            }
            catch (std::exception &e){
                hasAllow = false;
                std::cerr   <<  R"(Policy needs an "allow" block within the "attribute-filters" block.)"   <<  std::endl;
                // returns false because
                return false;
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
            std::cerr   <<  "Policy needs to have an \"attribute-filters\" section"   <<  std::endl;
            return false;
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

        // todo: add this check to generation of ABE policy after rewrite
        if (isAllowed && (allowedStreams.empty() && allowedAttributes.empty())) {
            std::cerr   <<  "\"allow\" section needs at least one attribute"        <<  std::endl;
            return false;
        }
        return true;
    }

    bool PolicyParser::generateABEPolicy() {
        std::list<std::string> policy;

        // stream name processing

        std::list<std::string> workingStreams;

        // REMOVED check for if allowedStreams is empty

        // todo: include check for allowed

//        when denying a stream, if that stream's parent is not allowed, then no change to ABE policy is made
//
//        add warning where child of allowed stream is also allowed (redundant)
        if (allowedStreams.empty()) {
            prefix = getPrefix(allowedAttributes.front());
            policy.push_back(prefix);
        }

        // add everything under all allowed stream names
        if (!allowedStreams.empty() || !deniedStreams.empty()) {
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
            policy.emplace_back(doStringThing(workingStreams, "OR"));
        }

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
                std::cerr   <<  "Cannot deny all given attributes"  <<  std::endl;
                return false;
            }
        }

        // todo: add checks for denied and allowed attributes to make sure there are no collisions
        // THIS SECTION WOULD ONLY CHANGE BY ADDING A FEW MORE CHECKS FOR STREAM NAMES

        // putting it all all together
        // AND together all separate conditions made for the output policy
        abePolicy = doStringThing(policy, "AND");

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

    bool PolicyParser::isAlike(std::string &attribute, std::string &checking) {
        int matches = 0;
        std::list<std::string> first = split(attribute, "/");
        std::list<std::string> second = split(checking, "/");

//        std::string workingFirst, workingSecond;

        // todo: change this for /org/md2k instead of /org.md2k/
        // removes first element if the attributes have leading "/"
        if (first.front().empty()) {
            first.remove(first.front());
        }
        if (second.front().empty()) {
            second.remove(second.front());
        }

        for (int n = 1; n < 3; n++) {
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



    // splitting string into list of strings along delimeter
    std::list<std::string> PolicyParser::split(const std::string &basicString, const std::string &delimeter) {
        std::list<std::string> splittedString;
        int startIndex = 0;
        int endIndex;
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

    std::string PolicyParser::trim(const std::string &basicString) {
        size_t first = basicString.find_first_not_of(' ');

        // empty string
        if (std::string::npos == first) {
            return basicString;
        }
        size_t last = basicString.find_last_not_of(' ');
        return basicString.substr(first, (last - first + 1));
    }

    const std::string &PolicyParser::getABEPolicy() const {
        return abePolicy;
    }

    // printing for PolicyParser object
    std::ostream &operator<<(std::ostream &os, const PolicyParser &parser) {
        os <<
        "PolicyParser Object "      <<  std::endl   <<
        "\t"    <<  "policy info"   <<  std::endl   <<
//        "\t\t"  <<  "configFilePath"    <<  "\t\t"  <<  parser.configFilePath   <<  std::endl   <<
//        "\t\t"  <<  "hasFilters"        <<  "\t\t"  <<  parser.hasFilters       <<  std::endl   <<
        "\t\t"  <<  "policyID"          <<  "\t\t"  <<  parser.policyID         <<  std::endl   <<
        "\t\t"  <<  "requesterNames"      <<  "\t\t"  ;
        for (const auto &item : parser.requesterNames) {
            os  <<  item        <<  " " ;
        }
        os      <<  std::endl   <<
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

    std::string PolicyParser::getPrefix(const std::string &ndnName) {
        std::string output;
        std::list<std::string> splitted = split(ndnName, "/");
        int index = 0;
        for (const auto &item: splitted) {
            if (index == 2) {
                break;
            }
            if (item.empty()){
                continue;
            }
            output.append("/" + item);
            index ++;
        }
        output.append("/");

        if (index < 2) {
            std::cerr << "Warning: Tried to get prefix for ndn name of less than 2 levels" << std::endl;
        }

        return output;
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
