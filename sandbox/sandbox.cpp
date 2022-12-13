#include "iostream"
#include "../src/server/util/name-tree.cpp"
#include "../src/controller/parser.cpp"

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

    auto a = nameTree.getNode(nameTree.getTreeRoot(), "/else/ndn/org/md2k/mguard/dd40c/phone/accelerometer");
    std::cout << (a == nullptr) << std::endl;
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
