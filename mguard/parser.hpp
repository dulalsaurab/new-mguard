#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>

namespace mguard {
    using ConfigSection = boost::property_tree::ptree;
    class PolicyParser {
    public:
        processFile();

        bool load(std::istream& input);

        bool processSection(const std::string &sectionName, const ConfigSection &section);

        bool processSectionNoSection(const std::string &sectionName, const ConfigSection &section);

        bool processSectionDataWindow(const std::string &sectionName, const ConfigSection &section);

    private:
        /*! m_confFileName The full path of the configuration file to parse. */
        std::string m_confFileName;
    };
}