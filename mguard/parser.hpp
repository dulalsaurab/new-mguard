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
        explicit PolicyParser(std::string &fileName);

        static bool isValidKey(const std::string& key);

        bool processFile();

        static std::list<std::string> split(const std::string& basicString, const std::string& delimiter);

        friend std::ostream &operator<<(std::ostream &os, const PolicyParser &parser);

    private:

        bool loadAndParse(std::istream& input);

        // full path of the config/policy file
        std::string configFilePath;

        // if no data window, all from stream is allowed
        bool hasFilters{}, hasAllow{}, hasDeny{};

        int policyID{};
        std::list<std::string> dataRequesterIDs; // this should be a list or array of some sort
        std::string dataStreamName;

        std::list<attributeFilter> filters;

        bool processAttributeFilter(ConfigSection &section, bool isAllowed);
    };

}