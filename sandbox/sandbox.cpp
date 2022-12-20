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

using NameTree = mguard::util::nametree::NameTree;
void testNametree() {
    NameTree nameTree;

    auto leaves = {"/else/ndn/org/md2k/mguard/dd40c/phone/accelerometer", "/else/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location", "/else/ndn/org/md2k/mguard/dd40c/phone/gyroscope", "/else/ndn/org/md2k/mguard/dd40c/phone/battery", "/else/ndn/org/md2k/mguard/dd40c/phone/gps", "/else/five/ndn/org/md2k/mguard/dd40c/phone/accelerometer", "/else/five/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location", "/else/five/ndn/org/md2k/mguard/dd40c/phone/gyroscope", "/else/ndn/five/org/md2k/mguard/dd40c/phone/battery", "/else/ndn/five/org/five/md2k/mguard/dd40c/phone/gps", "/else/ndn/four/org/md2k/five/mguard/dd40c/phone/accelerometer", "/else/ndn/four/org/md2k/mguard/five/dd40c/data_analysis/gps_episodes_and_semantic_location", "/else/ndn/four/org/md2k/mguard/dd40c/phone/gyroscope", "/ndn/four/org/md2k/mguard/dd40c/phone/battery", "/ndn/four/org/md2k/mguard/dd40c/phone/gps", "/else/else/something/ndn/org/md2k/ATTRIBUTE/activity/unknown", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/home", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/work", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/commuting", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/casino", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/oakland", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/SoCal", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/gym", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/shopping-mall", "/else/else/something/ndn/org/md2k/ATTRIBUTE/location/unknown", "/else/else/something/ndn/org/md2k/ATTRIBUTE/smoking/yes", "/else/else/something/ndn/org/md2k/ATTRIBUTE/smoking/no", "/else/else/something/ndn/org/md2k/ATTRIBUTE/activity/walking", "/else/else/something/ndn/org/md2k/ATTRIBUTE/activity/running", "/else/else/something/ndn/org/md2k/ATTRIBUTE/activity/eating", "/else/else/something/ndn/org/md2k/ATTRIBUTE/activity/drinking", "/else/else/something/ndn/org/md2k/ATTRIBUTE/activity/sleeping", "/something/something"};


    for (const auto &leaf : leaves) {
        nameTree.insertName(leaf);
    }
//
//    std::vector<ndn::Name> denied = {"/ndn/org/md2k/ATTRIBUTE/smoking/no", "/ndn/org/md2k/ATTRIBUTE/location/home"};
//
////     fixme: how to determine whether it was found
////    auto deniedNode = attStreamsTree.getNode(attStreamsTree.getTreeRoot(), denied);
//    auto a = attStreamsTree.getLeaves("/ndn/org/md2k/", denied);
//    for (const auto &item: a) {
//        std::cout << item << std::endl;
//    }

//    ndn::Name b = "ATTRIBUTE";
    auto a = nameTree.getNode(nameTree.getTreeRoot(), "/else/ndn/org/md2k/mguard/dd40c/phone/accelerometer");
//    nameTree._printTree(nameTree.getTreeRoot());
    std::cout << (a == nullptr) << std::endl;
}

//NameTree
//subtract(NameTree aye, NameTree bee){
//    auto leaves = bee.getLeaves()
//}

void printOutties(std::map<std::string, std::map<std::string, std::map<std::string, std::list<std::string>>>> outties, std::string stream) {
    std::cout << stream << ":" << std::endl;
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
    std::cout << std::endl;
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

    std::list<std::string> allow = {"/org/prefix-a/", "/org/prefix-b/", "/activity/walking"};
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
}

std::pair<int, int> getCurrentDay(int currentTime) {
    int days = currentTime / 86400;
    int startDay = days * 86400;
    int endDay = startDay + 86400;
    return {startDay, endDay};
}


std::vector<std::string> subtraction(std::vector<std::string> a, std::vector<std::string> b) {
    // does a - b
    std::vector<std::string> out;
    std::sort(a.begin(), a.end());
    return out;
}

int main(){
    std::cout << "sandbox run:" << std::endl;
    testParser();
    return 0;
}
