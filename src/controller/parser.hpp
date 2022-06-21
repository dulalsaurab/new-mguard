#ifndef MGUARD_PARSER_HPP
#define MGUARD_PARSER_HPP

//#include "common.hpp"

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

using ConfigSection = boost::property_tree::ptree;

class PolicyParser
{
public:
  explicit PolicyParser(std::basic_string<char> availableStreams);

  void
  parseAvailableStreams(const std::basic_string<char>& streamsFilePath);

  PolicyDetail
  parsePolicy(const std::basic_string<char>& policyFilePath);

  static std::pair<std::string, std::string>
  parseAttribute(std::string attribute);

private:
  bool
  generateABEPolicy();

  static std::list<std::string>
  splitRequesters(const std::string& basicString);

  static std::string 
  doStringThing(const std::list<std::string>& list, const std::string& operation);

  static std::list<std::string> 
  split(const std::string& basicString, const std::string& delimeter);


  void
  parsePolicy(std::istream &input);

  void
  processAttributeFilter(ConfigSection &section, bool isAllowed);

  static bool
  isAlike(std::string& attribute, std::string& checking);

  static std::string 
  processAttributes(const std::list<std::string>& attrList);

  // full path of the config/policy file
  std::string availableStreamsPath;

  std::list<std::string> requesterNames; // this should be a list or array of some sort
  std::string policyID, abePolicy;

  std::list<std::string> calculatedStreams, allowedStreams, allowedAttributes, deniedStreams, deniedAttributes;
  std::list<std::string> availableStreamLevels, availableStreams, allowedRequesters, availableAttributes;

};

} // namespace parser
} // namespace mguard

#endif // MGUARD_PARSER_HPP