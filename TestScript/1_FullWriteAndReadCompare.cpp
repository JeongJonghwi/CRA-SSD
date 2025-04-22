#include "Logger.h"
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

    string Run(Logger* logger) override
    {
        for (int groupStart = SSD_MINIMUM_ADDRESS; groupStart < SSD_MAXIMUM_ADDRESS; groupStart += 5) {
            std::stringstream ss;
            ss << std::hex << std::setw(2) << std::setfill('0') << groupStart;
            string value = "0x123456" + ss.str();
            if (groupWriteAndReadCompare(logger, groupStart, groupStart + 5, value) == "FAIL")
                return "FAIL";
        }

        return "PASS";
    }

private:
    SSD* ssd;

    string groupWriteAndReadCompare(Logger* logger, int startAddr, int endAddr, string value)
    {
        for (int addr = startAddr; addr < endAddr; addr++) {
            if (logger != nullptr)
                logger->Print("1_FullWriteAndReadCompare.write()", "write value " + value + " at " + std::to_string(addr));
            ssd->write(addr, value);
        }

        for (int addr = startAddr; addr < endAddr; addr++) {
            if (didReadFail(ssd->read(addr), value)) {
                if (logger != nullptr)
                    logger->Print("1_FullWriteAndReadCompare.readCompare()", "fail - expected " + value + " but wrong value at " + std::to_string(addr));
                return "FAIL";
            }
        }
        return "PASS";
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