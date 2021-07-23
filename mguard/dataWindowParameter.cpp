//
// Created by adamt on 7/22/2021.
//

#include "dataWindowParameter.h"

dataWindowParameter::dataWindowParameter(bool isAllowed, const std::string &key, const std::string &value)
        :isAllowed(isAllowed)
        ,key(key)
        ,value(value)
        {}

void dataWindowParameter::print() {
    std::cout << "isAllowed: " << isAllowed << " key: " << key << " value: " << value;
}

bool dataWindowParameter::isValidKey(std::string key) {
    std::list<std::string> allowedKeys = {"StreamName", "ColumnName", "ColumnNameValue"};
    if (std::find(allowedKeys.begin(), allowedKeys.end(), key) == std::end(allowedKeys)){
        return false;
    }
    return true;
}

columnNameValue::columnNameValue(bool isAllowed, const std::string &columnName, const std::string &value)
:dataWindowParameter(isAllowed, "columnNameValue", value),
columnName(columnName)
{}

void columnNameValue::print() {
    dataWindowParameter::print();
    std::cout << "columnName: " << columnName;
}
