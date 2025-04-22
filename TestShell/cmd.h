#pragma once
#include "Logger.h"
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

class CMD {
public:
    CMD()
        : input { "" }
        , address { 0 }
        , data { "" }
        , size { 0 } { };

    bool validCheck(string command);

    string getCommand()
    {
        return input;
    }
    int getAddress()
    {
        return address;
    }
    string getValue()
    {
        return data;
    }
    int getSize()
    {
        return size;
    }

private:
    string input;
    int address;
    string data;
    int size;
    Logger& logger = Logger::getInstance();

    int lbaLimitRange(int i);
    void setValidEraseSize(int s);
    void setValidEraseRangeSize(int startLba, int endLba);
    bool isValidLBA(string lba);
    bool isValidAddress(uint32_t address);
    bool isValidValue(string value);
    vector<string> splitBySpace(const string& input);
    bool isNumber(string s);
};