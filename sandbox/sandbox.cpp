#include "iostream"
#include "../src/controller/parser.cpp"
int main(){
    std::cout << "sandbox run:" << std::endl;
    mguard::parser::PolicyParser p("../policies/available_streams");
    auto a = p.parsePolicy("../policies/policy1");
    return 0;
}
