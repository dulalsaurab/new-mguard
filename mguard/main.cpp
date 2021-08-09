#include "iostream"
#include "parser.cpp"

int main(int argc, char** argv){
    std::string configFilePath("default.policy"), availableStreamsFilePath ("policies/available_streams");

    // only allowing -f configFilePath to change policy file name
    // very crude way of doing this, but it works for now
    if (argc == 3) {
        std::string s = "-f";
        if ((s.compare(argv[1])) == 0) {
            configFilePath = argv[2];
        } else {
            return -1;
        }
    }

    mguard::PolicyParser pp(configFilePath, availableStreamsFilePath);

    if (!pp.processFiles()){
        return -1;
    }

    std::cout << pp << std::endl;

    return 0;

}