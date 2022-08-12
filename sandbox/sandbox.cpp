#include "iostream"
#include "../src/server/util/name-tree.cpp"
#include "../src/controller/parser.cpp"
#include "map"

void testParser() {
    mguard::parser::PolicyParser p("../policies/available_streams");
    mguard::parser::PolicyDetail a = p.parsePolicy("../policies/policy1");

    // policy id
    std::cout << "policy id:" << std::endl;
    std::cout << "\t" << a.policyIdentifier << std::endl;

    // requesters
    std::cout << "requesters:" << std::endl;
    for (const auto &requester: a.requesters) {
        std::cout << "\t" << requester << std::endl;
    }

    // streams
    std::cout << "streams:" << std::endl;
    for (const auto &stream: a.streams) {
        std::cout << "\t" << stream << std::endl;
    }

    // abe policy
    std::cout << "abe policy:" << std::endl;
    std::cout << "\t" << a.abePolicy << std::endl;
}

void testNametree() {
    mguard::util::nametree::NameTree nameTree;

    auto leaves = {"/ndn/org/md2k/ATTRIBUTE/location/home", "/ndn/org/md2k/ATTRIBUTE/location/work", "/ndn/org/md2k/ATTRIBUTE/location/commuting", "/ndn/org/md2k/ATTRIBUTE/location/casino", "/ndn/org/md2k/ATTRIBUTE/location/oakland", "/ndn/org/md2k/ATTRIBUTE/location/SoCal", "/ndn/org/md2k/ATTRIBUTE/location/gym", "/ndn/org/md2k/ATTRIBUTE/location/shopping-mall", "/ndn/org/md2k/ATTRIBUTE/smoking/yes", "/ndn/org/md2k/ATTRIBUTE/smoking/no", "/ndn/org/md2k/ATTRIBUTE/activity/walking", "/ndn/org/md2k/ATTRIBUTE/activity/running", "/ndn/org/md2k/ATTRIBUTE/activity/eating", "/ndn/org/md2k/ATTRIBUTE/activity/drinking", "/ndn/org/md2k/ATTRIBUTE/activity/sleeping"};

    for (const auto &leaf : leaves) {
        nameTree.insertName(leaf);
    }

    ndn::Name denied = "/ndn/org/md2k/ATTRIBUTE/smoking/yes";

    // fixme: how to determine whether it was found
    auto deniedNode = nameTree.search(nameTree.getTreeRoot(), denied);
}

void printOutties(std::map<std::string, std::map<std::string, std::map<std::string, std::list<std::string>>>> outties, std::string stream) {
    std::cout << "things for stream:\t" << stream << std::endl;
    std::cout << "\tallowed streams\t" << std::endl;
    for (const auto& item : outties[stream]["allowed"]["stream"]) {
        std::cout << "\t\t" << item << std::endl;
    }
    std::cout << "\tallowed attributes" << std::endl;
    for (const auto& item : outties[stream]["allowed"]["attribute"]) {
        std::cout << "\t\t" << item << std::endl;
    }
    std::cout << "\tdenied streams" << std::endl;
    for (const auto& item : outties[stream]["denied"]["stream"]) {
        std::cout << "\t\t" << item << std::endl;
    }
    std::cout << "\tdenied attributes" << std::endl;
    for (const auto& item : outties[stream]["denied"]["attribute"]) {
        std::cout << "\t\t" << item << std::endl;
    }
}

void additionalParser() {
    std::map<std::string, std::list<std::string>> map;
    for (const auto thing : {"/activity/running", "/activity/walking", "/activity/undefined", "/location/home", "/location/gym", "/location/work", "/location/undefined"}) {
        map["/org/prefix-a/"].push_back(thing);
    }

    for (const auto thing : {"/smoking/yes", "/smoking/no"}) {
        map["/org/prefix-b/"].push_back(thing);
    }

    for (const auto thing : {"/location/home", "/location/gym", "/location/work", "/location/undefined", "/smoking/yes", "/smoking/no"}) {
        map["/org/prefix-c/"].push_back(thing);
    }

    std::list<std::string> allow = {"/org/prefix-a/", "/org/prefix-b/", "/activity/walking", "/org/prefix-c/", "/smoking/yes"};
    std::list<std::string> deny = {"/location/home"};

    // outties[stream][allowed][attribute/stream]
    std::map<std::string, std::map<std::string, std::map<std::string, std::list<std::string>>>> outties;

    for (const auto &thing: map) {
        std::string stream = thing.first;
        std::list<std::string> attributes = thing.second;

        for (const auto &filter: allow) {
            // check if it matches the thing[0] (stream)
            if (stream.find(filter) == 0) {
                std::cout << stream << "\t" << filter << std::endl;
                outties[stream]["allowed"]["stream"].push_back(filter);
            } else if (std::find(attributes.begin(), attributes.end(), filter) != std::end(attributes)) {
                outties[stream]["allowed"]["attribute"].push_back(filter);
            }
            // check if it matches the thing[1] (attribute)
        }
        for (const auto &filter: deny) {
            // check if it matches the thing[0] (stream)
            if (stream.find(filter) == 0) {
                std::cout << stream << "\t" << filter << std::endl;
                outties[stream]["denied"]["stream"].push_back(filter);
            } else if (std::find(attributes.begin(), attributes.end(), filter) != std::end(attributes)) {
                outties[stream]["denied"]["attribute"].push_back(filter);
            }
        }
    }

    printOutties(outties, "/org/prefix-a/");
    printOutties(outties, "/org/prefix-b/");
    printOutties(outties, "/org/prefix-c/");
}

int main(){
    std::cout << "sandbox run:" << std::endl;
    testParser();
    return 0;
}
