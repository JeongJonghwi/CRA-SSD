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

    string Run() override
    {
        ssd->erase(0, 3);
        for (int i = 0; i < 30; i++) {
            for (int startAddr = 2; startAddr <= 96; startAddr += 2) {
                ssd->write(startAddr, "0x12345678");
                ssd->write(startAddr, "0x87654321");
                ssd->erase(startAddr, 3);

                if (didReadFail(ssd->read(startAddr), "0x00000000"))
                    return "FAIL";

                if (didReadFail(ssd->read(startAddr + 1), "0x00000000"))
                    return "FAIL";

                if (didReadFail(ssd->read(startAddr + 2), "0x00000000"))
                    return "FAIL";
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