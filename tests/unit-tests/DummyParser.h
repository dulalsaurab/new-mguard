//
// Created by adamt on 12/19/2022.
//

#include <controller/parser.hpp>
using namespace ndn;

#ifndef MGUARD_DUMMYPARSER_H
#define MGUARD_DUMMYPARSER_H

namespace mguard {
namespace parser {
namespace tests {

class DummyParser : public PolicyParser {
public:
    explicit DummyParser(const std::basic_string<char>& availableStreams);

    void
    parseAvailableStreams(const std::basic_string<char>& streamsFilePath, NameTree& streamsAttributes, NameTree& requesters);

    PolicyDetail
    parsePolicy(const std::basic_string<char>& policyFilePath);

    static std::list<std::string>
    getFilters(ConfigSection &section);

    static std::pair<std::string, std::string>
    parseAttribute(const std::string& attribute);

    static std::list<std::string>
    splitRequesters(const std::string& basicString);

    static std::string
    doStringThing(const std::list<std::string>& list, const std::string& operation);

    static std::list<std::string>
    split(const std::string& basicString, const std::string& delimiter);

    static std::string
    processAttributes(const std::list<std::string>& attrList);

    ParsedSection
    parseSection(ConfigSection& section);

    SectionDetail
    calculatePolicy(const ParsedSection& section);

    // information from the available-streams file
    NameTree attStreamsTree, requestersTree;

};

} // mguard
} // parser
} // tests

#endif //MGUARD_DUMMYPARSER_H
