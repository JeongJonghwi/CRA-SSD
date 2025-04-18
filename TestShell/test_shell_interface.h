#pragma once
#include <string>

using std::string;


class ITestShell {
public:
    virtual string read(uint32_t address) = 0;
    virtual string write(uint32_t address, string value) = 0;
    virtual string fullWrite(string value) = 0;
    virtual string fullRead() = 0;
};