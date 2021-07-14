#include "parser.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>
#include <fstream>


using namespace bf = boost::filesystem;
using namespace std;

namespace mguard {
    template<class T>
    class ConfigVariable{
    public:
        ConfigVariable(const string& key): m_key(key){

        }

        ConfigVariable(const string& key){

        }

        bool parseConfigSection(const ConfigSection& section){
            T value = section.get<T>(m_key);
            return true;

        }

    private:
        const string m_key;
        T value;
    };
    bool PolicyParser::processFile() {
        ifstream inputFile (m_confFileName);
        if (!inputFile.is_open()){
            string msg = "File reading didn't work on " + m_confFileName;
            cerr << msg << endl;
            return false;
        }
        bool out = load(inputFile);
        inputFile.close();

        return out;
    }
    bool PolicyParser::load(istream& input) {
        return true;
    }

    bool PolicyParser::processSection(const string &sectionName, const string &section) {
        if (sectionName == "none"){
            return processSectionNoSection(section);
        }
        if (sectionName == "data-window"){
            return processSectionDataWindow(section);
        }
    }

    bool PolicyParser::processSectionNoSection(const string &sectionName, const string &section) {
        return true;
    }
    bool PolicyParser::processSectionDataWindow(const string &sectionName, const string &section) {
        // process allow section

        // process deny section
        return true;
    }
}
