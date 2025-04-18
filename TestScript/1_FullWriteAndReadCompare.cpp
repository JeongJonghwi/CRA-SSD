#include "pch.h"
#include "test_script_interface.h"
#include "ssd_interface.h"

#include <iostream>

class FullWriteAndReadCompareScript : public ITestScript {
public:
    FullWriteAndReadCompareScript(SSD* ssd)
        : ssd(ssd) {}

    string Run() override
    {
        ssd->read(0);
        return "Run In DLL";
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