#include "ssd_interface.h"
#include "gmock/gmock.h"
#include <string>
#include <vector>

class TestScript {
public:
    TestScript(SSD* ssd)
        : ssd(ssd)
    {
    }

    string fullWriteAndReadCompare()
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

    string partialLBAWrite(string value)
    {
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

    string writeReadAging(string value)
    {
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

    string eraseAndWriteAging()
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

    bool didReadFail(string actual, string expected)
    {
        std::transform(expected.begin() + 2, expected.end(), expected.begin() + 2, ::toupper);
        if (actual != expected)
            return true;
        return false;
    }

    bool isValidValue(string value)
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