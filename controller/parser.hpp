#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ostream>

namespace mguard {

using ConfigSection = boost::property_tree::ptree;

class attributeFilter {

public:
  attributeFilter(bool isAllowed, std::string attribute);
  friend std::ostream &operator<<(std::ostream &os, const attributeFilter &parameter);
  std::string attribute;

private:
  bool isAllowed;
};

class PolicyParser 
{
public:
  PolicyParser(std::basic_string<char> configFilePath, std::basic_string<char> availableStreams);

  bool 
  parseFiles();

  bool 
  generateABEPolicy();

  friend std::ostream &operator<<(std::ostream &os, const PolicyParser &parser);

private:
  static std::list<std::string> 
  splitRequesters(const std::string& basicString);

  static std::string 
  doStringThing(const std::list<std::string>& list, const std::string& operation);

  static std::list<std::string> 
  split(const std::string& basicString, const std::string& delimeter);

  bool 
  parseAvailableStreams(std::istream &input);

  bool 
  parsePolicy(std::istream &input);

  bool 
  processAttributeFilter(ConfigSection &section, bool isAllowed);

  // if no data window, all from stream is allowed
  bool 
  hasDeny{};

  static bool 
  isAlike(std::string& attribute, std::string& checking);

  static std::string 
  processAttributes(const std::list<std::string>& attrList);

  // full path of the config/policy file
  std::string configFilePath, availableStreamsPath;

  int policyID{};
  std::list<std::string> requesterNames; // this should be a list or array of some sort
  std::string abePolicy;

  std::list<attributeFilter> filters;
  std::list<std::string> allowedStreams, allowedAttributes, deniedStreams, deniedAttributes;
  std::list<std::string> availableStreamLevels, availableStreams, allowedRequesters, availableAttributes;
  
};

}