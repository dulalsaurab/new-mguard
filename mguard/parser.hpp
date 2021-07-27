#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ostream>
//#include "dataWindowParameter.hpp"

namespace mguard {
    class dataWindowParameter {
    private:
        bool isAllowed;
        std::string key, value, streamName, columnName;
    public:
        dataWindowParameter(bool isAllowed, std::string key, std::string value);

        dataWindowParameter(bool isAllowed, std::string streamName, std::string columnName, std::string value);

        static bool isValidKey(const std::string& key);

        friend std::ostream &operator<<(std::ostream &os, const dataWindowParameter &parameter);

    };
    using ConfigSection = boost::property_tree::ptree;
    class PolicyParser {
    public:
        explicit PolicyParser(std::string &fileName);

        bool processFile();

        static std::list<std::string> split(const std::string& basicString, const std::string& delimiter);

        friend std::ostream &operator<<(std::ostream &os, const PolicyParser &parser);

    private:

        bool loadAndParse(std::istream& input);

        // full path of the config/policy file
        std::string configFilePath;

        // if no data window, all from stream is allowed
        bool hasDataWindow;

        int policyID;
        std::string studyID;
        std::string dataOwnerID;
        std::list<std::string> dataRequesterIDs; // this should be a list or array of some sort
        std::string dataStreamName;

        std::list<dataWindowParameter> parameters;

        void printForDebug();

        bool processDWSection(ConfigSection &section, bool isAllowed);
    };

}