#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ostream>

namespace mguard {
    using ConfigSection = boost::property_tree::ptree;
    class PolicyParser {
    public:
        explicit PolicyParser(std::string &fileName);

        bool processFile();

    private:
        bool loadAndParse(std::istream& input);

        bool processSectionAllow(ConfigSection &section);

        bool processSectionDeny(ConfigSection &section);

        // full path of the config/policy file
        std::string m_confFileName;

        int policyID;
        std::string studyID;
        std::string dataOwnerID;
        std::string dataRequesterIDs; // this should be a list or array of some sort
        std::string dataStreamName;

        void printForDebug();
    };
}