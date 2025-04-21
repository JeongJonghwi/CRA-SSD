#pragma once
#include <algorithm>
#include <iostream>

class SSD;
class Logger;

class ITestScript {
public:
    virtual ~ITestScript() { }
    virtual std::string Run(Logger* logger) = 0;

protected:
    const int SSD_MINIMUM_ADDRESS = 0;
    const int SSD_MAXIMUM_ADDRESS = 100;

    bool didReadFail(std::string actual, std::string expected)
    {
        std::transform(expected.begin() + 2, expected.end(), expected.begin() + 2, ::toupper);
        if (actual != expected)
            return true;
        return false;
    }

    bool isValidValue(std::string value)
    {
        if (value.length() != 10 || value[0] != '0' || value[1] != 'x')
            return false;

        for (int i = 2; i < 10; i++) {
            if ((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F') || (value[i] >= 'a' && value[i] <= 'f'))
                continue;
            return false;
        }
        return true;
    }
};