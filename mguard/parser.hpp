#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ostream>
//#include "attributeFilter.hpp"

namespace mguard {
    class attributeFilter {
    private:
        bool isAllowed;
    public:
        attributeFilter(bool isAllowed, std::string attribute);

        friend std::ostream &operator<<(std::ostream &os, const attributeFilter &parameter);

        std::string attribute;
    };
    using ConfigSection = boost::property_tree::ptree;
    class PolicyParser {
    public:
        PolicyParser(std::basic_string<char> configFilePath, std::basic_string<char> availableStreams);

        bool parseFiles();

        static std::list<std::string> split(const std::string& basicString, const std::string& delimeter);

        static std::string trim(const std::string& basicString);

        static std::string doStringThing(const std::list<std::string>& list, const std::string& operation);

        friend std::ostream &operator<<(std::ostream &os, const PolicyParser &parser);

        const std::string &getABEPolicy() const;

        static std::list<std::string> splitRequesters(const std::string& basicString);

    private:
        bool parseAvailableStreams(std::istream &input);
        std::list<std::string> availableStreamLevels, availableStreams, allowedRequesters, availableAttributes;

        bool parsePolicy(std::istream &input);

        bool processAttributeFilter(ConfigSection &section, bool isAllowed);

        bool generateABEPolicy();

        // full path of the config/policy file
        std::string configFilePath, availableStreamsPath;

        // if no data window, all from stream is allowed
        bool hasAllow{}, hasDeny{};

        int policyID{};
        std::list<std::string> requesterNames; // this should be a list or array of some sort
        std::string abePolicy;

        std::list<attributeFilter> filters;
        std::list<std::string> allowedStreams, allowedAttributes, deniedStreams, deniedAttributes;

        static bool isAlike(std::string& attribute, std::string& checking);

        static std::string processAttributes(const std::list<std::string>& attrList);
    };

}