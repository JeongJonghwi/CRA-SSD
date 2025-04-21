#include "pch.h"
#include "ssd_interface.h"
#include "test_script_interface.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <vector>

class WrtieReadAgingScript : public ITestScript {
public:
    WrtieReadAgingScript(SSD* ssd)
        : ssd(ssd)
    {
    }

    string Run() override
    {
        string value = randomValue();

        if (!isValidValue(value))
            return "FAIL";

        for (int i = 0; i < 200; i++) {
            ssd->write(0, value);
            ssd->write(99, value);
            if (didReadFail(ssd->read(0), ssd->read(99)))
                return "FAIL";
        }
        return "PASS";
    }

private:
    SSD* ssd;

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

    string randomValue()
    {
        unsigned int random_value = static_cast<unsigned int>(std::rand());
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << random_value;
        std::string hexStr = "0x" + ss.str();
        return hexStr;
    }
};

extern "C" __declspec(dllexport) ITestScript* CreateScript_3_(SSD* ssd)
{
    return new WrtieReadAgingScript(ssd);
}

extern "C" __declspec(dllexport) ITestScript* CreateScript_3_WriteReadAging(SSD* ssd)
{
    return new WrtieReadAgingScript(ssd);
}