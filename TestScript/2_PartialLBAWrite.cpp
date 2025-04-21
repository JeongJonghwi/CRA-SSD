#include "pch.h"
#include "ssd_interface.h"
#include "test_script_interface.h"

#include <iostream>
#include <vector>

class PartialWriteAndReadCompareScript : public ITestScript {
public:
    PartialWriteAndReadCompareScript(SSD* ssd)
        : ssd(ssd)
    {
    }

    string Run() override
    {
        string value = "0x12345678";

        if (!isValidValue(value))
            return "FAIL";

        std::vector<int> order = { 4, 0, 3, 1, 2 };

        for (int i = 0; i < 30; i++) {
            for (auto o : order) {
                ssd->write(o, value);
            }

            for (auto o : order) {
                if (didReadFail(ssd->read(o), value))
                    return "FAIL";
            }
        }
        return "PASS";
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