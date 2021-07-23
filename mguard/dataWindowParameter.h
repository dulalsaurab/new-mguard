//
// Created by adamt on 7/22/2021.
//

#ifndef MGUARD_DATAWINDOWPARAMETER_H
#define MGUARD_DATAWINDOWPARAMETER_H


#include <ostream>

class dataWindowParameter {
private:
    bool isAllowed;
    std::string key;
    std::string value;
public:
    dataWindowParameter(bool isAllowed, const std::string &key, const std::string &value);

    static bool isValidKey(std::string key);

    void print();

};

class columnNameValue : public dataWindowParameter{
public:
    columnNameValue(bool isAllowed, const std::string &columnName, const std::string &value);

    void print();

private:
    std::string columnName;
};

#endif //MGUARD_DATAWINDOWPARAMETER_H
