#include "parser.hpp"

#include "boost/property_tree/info_parser.hpp"

#include <iostream>

namespace pt = boost::property_tree;

namespace mguard {
    PolicyParser::PolicyParser(std::string &fileName)
    :m_confFileName(fileName)
    {}

    bool PolicyParser::processFile() {
        // input for file
        std::ifstream inputFile (m_confFileName.c_str());
        if (!inputFile.is_open()){
            std::string msg = "ifstream input failed for " + m_confFileName;
            std::cerr << msg << std::endl;
            return false;
        }
        bool out = loadAndParse(inputFile);
        inputFile.close();

        return out;
    }
    bool PolicyParser::loadAndParse(std::istream& input) {
        // loading input file into sections
        ConfigSection section;
        try{
            pt::read_info(input, section);
        } catch (pt::info_parser_error& error) {
            std::cerr << "read_info failed" << std::endl;
            return false;
        }
        try {
            // set all instance vairables (all required in policy)
            policyID = section.get<int>("POLICY-ID");
            studyID = section.get<std::string>("STUDY-ID");
            dataOwnerID = section.get<std::string>("DATA-OWNER-ID");
            dataRequesterIDs = section.get<std::string>("DATA-REQUESTER-IDs");
            dataStreamName = section.get<std::string>("DATA-STREAM-NAME");
        } catch (const std::exception &exception) {
            // this is usually a syntax error within the policy
            std::cerr << exception.what() << std::endl;
            return false;
        }
        // used for debugging to make sure instance variables are set correctly
        printForDebug();

        // this implementation means it's all required
        pt::ptree dw = section.get_child("DATA-WINDOW");

        // I'm not sure of the best way to store the parts under ALLOW and DENY
        processSectionAllow(dw.get_child("ALLOW"));
        processSectionDeny(dw.get_child("DENY"));

        return true;
    }

    /* TODO: I'm still working on these two functions. I have a plan for getting the values.
     * TODO: I am working on getting the data stored. We can make changes to where it is stored once that's done
     * */
    bool PolicyParser::processSectionAllow(ConfigSection &section) {
        std::cout << section.get<std::string>("one") << "\n";

        return true;
    }

    bool PolicyParser::processSectionDeny(ConfigSection &section) {
        std::cout << section.get<std::string>("many") << "\n";

        return true;
    }

    void PolicyParser::printForDebug (){
        std::cout << "policyID: " << policyID << " studyID: " << studyID << " dataOwnerID: "
           << dataOwnerID << " dataRequesterIDs: " << dataRequesterIDs << " dataStreamName: "
           << dataStreamName << std::endl;
    }
}
