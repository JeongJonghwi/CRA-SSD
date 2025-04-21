#include "pch.h"
#include "test_script_interface.h"
#include "ssd_interface.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

class FullWriteAndReadCompareScript : public ITestScript {
public:
    FullWriteAndReadCompareScript(SSD* ssd)
        : ssd(ssd) {}

    string Run() override
    {
        for (int groupStart = SSD_MINIMUM_ADDRESS; groupStart < SSD_MAXIMUM_ADDRESS; groupStart += 5) {
            std::stringstream ss;
            ss << std::hex << std::setw(2) << std::setfill('0') << groupStart;
            string value = "0x123456" + ss.str();
            if (groupWriteAndReadCompare(groupStart, groupStart + 5, value) == "FAIL")
                return "FAIL";
        }

        return "PASS";
    }

    string groupWriteAndReadCompare(int startAddr, int endAddr, string value)
    {
        for (int addr = startAddr; addr < endAddr; addr++) {
            ssd->write(addr, value);
        }

        for (int addr = startAddr; addr < endAddr; addr++) {
            if (didReadFail(ssd->read(addr), value))
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
};

extern "C" __declspec(dllexport) ITestScript* CreateScript_1_(SSD* ssd)
{
    return new FullWriteAndReadCompareScript(ssd);
}

extern "C" __declspec(dllexport) ITestScript* CreateScript_1_FullWriteAndReadCompare(SSD* ssd)
{
    return new FullWriteAndReadCompareScript(ssd);
}