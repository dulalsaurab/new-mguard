#include "iostream"
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

int main(){
    std::cout << "sandbox run:" << std::endl;

    return 0;
}
