#include "pch.h"
#include "ssd_interface.h"
#include "test_script_interface.h"

#include <iomanip>
#include <iostream>
#include <sstream>

class FullWriteAndReadCompareScript : public ITestScript {
public:
    FullWriteAndReadCompareScript(SSD* ssd)
        : ssd(ssd)
    {
    }

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
};

extern "C" __declspec(dllexport) ITestScript* CreateScript_1_(SSD* ssd)
{
    return new FullWriteAndReadCompareScript(ssd);
}

extern "C" __declspec(dllexport) ITestScript* CreateScript_1_FullWriteAndReadCompare(SSD* ssd)
{
    return new FullWriteAndReadCompareScript(ssd);
}