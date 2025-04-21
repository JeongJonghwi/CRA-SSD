#include "pch.h"
#include "ssd_interface.h"
#include "test_script_interface.h"
#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

class WrtieReadAgingScript : public ITestScript {
public:
    WrtieReadAgingScript(SSD* ssd)
        : ssd(ssd)
    {
    }

    string Run(Logger* logger) override
    {
        for (int i = 0; i < 200; i++) {
            string value = randomValue();
            logger->Print("3_WriteReadAging.write()", "write value " + value + " at 0");
            ssd->write(0, value);
            logger->Print("3_WriteReadAging.write()", "write value " + value + " at 99");
            ssd->write(99, value);
            if (didReadFail(ssd->read(0), ssd->read(99))) {
                logger->Print("3_WriteReadAging.readCompare()", "fail - values mismatch");
                return "FAIL";
            }
        }
        return "PASS";
    }

private:
    SSD* ssd;

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