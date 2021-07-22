#include "iostream"
#include "parser.cpp"

int main(int argc, char** argv){
    std::string configFileName("default.policy");

    // only allowing -f configFileName to change policy file name
    // very crude way of doing this, but it works for now
    if (argc == 3) {
        std::string s = "-f";
        if ((s.compare(argv[1])) == 0) {
            configFileName = argv[2];
        } else {
            return -1;
        }
    }

    mguard::PolicyParser pp(configFileName);

    if (!pp.processFile()){
        return -1;
    }

    return 0;

}