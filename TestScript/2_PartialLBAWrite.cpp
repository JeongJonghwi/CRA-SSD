#include "pch.h"
#include "ssd_interface.h"
#include "test_script_interface.h"

#include <iostream>

class PartialWriteAndReadCompareScript : public ITestScript {
public:
    PartialWriteAndReadCompareScript(SSD* ssd)
        : ssd(ssd)
    {   
    }

    string Run() override
    {
        return "RUN 2_PartialLBAWrite";
    }

private:
    SSD* ssd;
};

extern "C" __declspec(dllexport) ITestScript* CreateScript_2_(SSD* ssd)
{
    return new PartialWriteAndReadCompareScript(ssd);
}

extern "C" __declspec(dllexport) ITestScript* CreateScript_2_PartialLBAWrite(SSD* ssd)
{
    return new PartialWriteAndReadCompareScript(ssd);
}