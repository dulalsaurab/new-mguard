#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ostream>
//#include "attributeFilter.hpp"

namespace mguard {
    class attributeFilter {
    private:
        bool isAllowed;
        std::string attribute;
    public:
        attributeFilter(bool isAllowed, std::string attribute);

        friend std::ostream &operator<<(std::ostream &os, const attributeFilter &parameter);
    };
    using ConfigSection = boost::property_tree::ptree;
    class PolicyParser {
    public:
        PolicyParser(std::string &configFilePath, std::string &availableStreams);

        bool processFiles();

        static std::list<std::string> split(const std::string& basicString, const std::string& delimiter);

        friend std::ostream &operator<<(std::ostream &os, const PolicyParser &parser);

    private:
        bool parseAvailableStreams(std::istream &input);
        std::list<std::string> availableStreams, availableUsers;

        bool parsePolicy(std::istream &input);

        bool prelimCheck();

        // full path of the config/policy file
        std::string configFilePath, availableStreamsPath;

        // if no data window, all from stream is allowed
        bool hasFilters{}, hasAllow{}, hasDeny{};

        int policyID{};
        std::list<std::string> requesterIDs; // this should be a list or array of some sort
        std::string streamName;

        std::list<attributeFilter> filters;

        bool processAttributeFilter(ConfigSection &section, bool isAllowed);
    };

}