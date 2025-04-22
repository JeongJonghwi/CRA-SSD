#include "Logger.h"
#include "pch.h"
#include "ssd_interface.h"
#include "test_script_interface.h"

#include <iostream>
#include <vector>

class EraseAndWriteAging : public ITestScript {
public:
    EraseAndWriteAging(SSD* ssd)
        : ssd(ssd)
    {
    }

    string Run(Logger* logger) override
    {
        if (logger != nullptr)
            logger->Print("4_EraseAndWriteAging.erase()", "erase value from 0 to 3");
        ssd->erase(0, 3);
        for (int i = 0; i < 30; i++) {
            for (int startAddr = 2; startAddr <= 96; startAddr += 2) {
                if (logger != nullptr)
                    logger->Print("4_EraseAndWriteAging.write()", "write value at " + std::to_string(startAddr));
                ssd->write(startAddr, "0x12345678");
                ssd->write(startAddr, "0x87654321");

                if (logger != nullptr)
                    logger->Print("4_EraseAndWriteAging.erase()", "erase value from " + std::to_string(startAddr) + " to " + std::to_string(startAddr + 2));
                ssd->erase(startAddr, 3);

                if (didReadFail(ssd->read(startAddr), "0x00000000")) {
                    if (logger != nullptr)
                        logger->Print("4_EraseAndWriteAging.erase()", "value is not erased at " + std::to_string(startAddr));
                    return "FAIL";
                }

                if (didReadFail(ssd->read(startAddr + 1), "0x00000000")) {
                    if (logger != nullptr)
                        logger->Print("4_EraseAndWriteAging.erase()", "value is not erased at " + std::to_string(startAddr + 1));
                    return "FAIL";
                }

                if (didReadFail(ssd->read(startAddr + 2), "0x00000000")) {
                    if (logger != nullptr)
                        logger->Print("4_EraseAndWriteAging.erase()", "value is not erased at " + std::to_string(startAddr + 2));
                    return "FAIL";
                }
            }
        }

        return "PASS";
    }

private:
    SSD* ssd;
};

extern "C" __declspec(dllexport) ITestScript* CreateScript_4_(SSD* ssd)
{
    return new EraseAndWriteAging(ssd);
}

extern "C" __declspec(dllexport) ITestScript* CreateScript_4_EraseAndWriteAging(SSD* ssd)
{
    return new EraseAndWriteAging(ssd);
}