#include "parser.hpp"
#include "boost/property_tree/info_parser.hpp"

#include <iostream>
#include <utility>

namespace pt = boost::property_tree;
namespace mguard {

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
  if (!inputStreams()) {
      std::cerr << "inputStreams failed" << std::endl;
  }

}

PolicyParser::policyDetails
PolicyParser::getPolicyInfo() {
    policyDetails a = {policyID, allowedStreams,allowedRequesters};
    return a;
}

bool PolicyParser::inputStreams(const std::basic_string<char>& streamsFilePath) {
    // todo: should probably reset everything that would be stored in parser
    // just so that you can't get old data after redoing global variables
    availableStreamsPath = streamsFilePath;
    return inputStreams();
}

bool
PolicyParser::inputStreams() {
    // input for available streams
    std::ifstream availableStreamsFile(availableStreamsPath.c_str());
    if (!availableStreamsFile.is_open()) {
        std::cerr << "ifstream input failed for " << availableStreamsPath << std::endl;
        availableStreamsFile.close();
        return false;
    }
    // parsing of available streams file
    if (!parseAvailableStreams(availableStreamsFile)) {
        std::cerr << "parsing of available-streams failed" << std::endl;
        availableStreamsFile.close();
        return false;
    }
    availableStreamsFile.close();

    return true;
}

bool
PolicyParser::inputPolicy(const std::basic_string<char>& policyFilePath) {
  // input for config file
  std::ifstream policyFile (policyFilePath.c_str());
  if (!policyFile.is_open()){
      std::cerr << "ifstream input failed for " << policyFilePath << std::endl;
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
  generateABEPolicy();
  return true;
}

bool PolicyParser::parseAvailableStreams(std::istream &input) 
{
  ConfigSection section;

  try {
    pt::read_info(input, section);
  } catch (pt::info_parser_error& error) {
    std::cerr   <<  "read_info failed for available-streams"  <<  std::endl;
    return false;
  }

  try 
  {
    // processing given streams and storing all possible streams
    std::list<std::string> levels;
    std::string buildingName;
    for (const auto &item : section.get_child("available-streams")) 
    {
      // add stream to list of streams
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

    } 
    catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &error) {
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
        policyID = section.get<std::string>("policy-id");
        auto raw = section.get<std::string>("requester-names");
        auto splitted = split(raw, ",");
        requesterNames = splitRequesters(section.get<std::string>("requester-names"));
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
    // NOTE: I should figure out better way to structure this part
    // this could possibly be done with section.get_child_optional()
    try {
        pt::ptree filterTree = section.get_child("attribute-filters");
        try {
            if (!processAttributeFilter(filterTree.get_child("allow"), true)) {
                return false;
            }
        }
        catch (std::exception &e){
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
            // probably better way of doing this, but I'm only setting that variable
            // because something needs to be in the catch block
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

bool PolicyParser::processAttributeFilter(pt::ptree &section, bool isAllowed) 
{
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

    if (allowedStreams.empty()) {
        std::cerr << "User needs at least one stream name in input policy" << std::endl;
        return false;
    }

    // warning variables
    std::list<std::string> allowDenyWarning;
    std::string warn;
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
            std::cout << warning << std::endl;
        }
        // error for if no streams are allowed
        if (workingStreams.empty()) {
            std::cerr << "ERROR: No streams allowed by policy" << std::endl;
            return false;
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

std::list<std::string> PolicyParser::splitRequesters(const std::string& basicString) 
{
    std::list<std::string> output;
    std::list<std::string> n = mguard::PolicyParser::split(basicString, ",");
    for (const auto& item : n) {
        for (const auto& thing : mguard::PolicyParser::split(item, " ")) {
            if (!thing.empty()) {
                output.push_back(thing);
            }
        }
    }
    return output;
}

// printing for PolicyParser object
std::ostream 
&operator<<(std::ostream &os, const PolicyParser &parser) {
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
