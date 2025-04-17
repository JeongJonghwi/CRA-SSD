#include "cmd.cpp"
#include "interface.h"
#include "gmock/gmock.h"
#include <vector>

class TestShell {
public:
    TestShell(SSD* ssd)
        : ssd(ssd)
    {
    }

    string read(uint32_t address)
    {
        if (!isValidAddress(address))
            return "ERROR";
        return ssd->read(address);
    }

    string write(uint32_t address, string value)
    {
        if (!isValidAddress(address))
            return "ERROR";
        if (!isValidValue(value))
            return "ERROR";

        ssd->write(address, value);
        return "Done";
    }

    string help()
    {
        string ret = "Team CodeCraft: ";
        ret += "°­µ¿Çö, ±èÅÂ¿Ï, À¯Çö½Â, ÀÌ¼öÁö, Á¤Á¾ÈÖ\n";
        ret += "write: write {value} to {address}\n";
        ret += "read: read from {address}\n";
        ret += "fullwrite: write {value} to 0x0 ~ 0x100\n";
        ret += "fullread: read from all 0x0 ~ 0x100\n";
        ret += "help: print this message\n";
        ret += "exit: exit TestShell\n";
        return ret;
    }

    string exit()
    {
        return "Exit TestShell";
    }

    string fullWrite(string value)
    {
        if (!isValidValue(value))
            return "ERROR";
        for (int i = SSD_MINIMUM_ADDRESS; i < SSD_MAXIMUM_ADDRESS; i++) {
            ssd->write(i, value);
        }
        return "Done";
    }

    string fullRead()
    {
        string ret = "";
        for (int i = SSD_MINIMUM_ADDRESS; i < SSD_MAXIMUM_ADDRESS; i++) {
            ret += ssd->read(i);
            ret += "\n";
        }
        return ret;
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

    bool isValidCommandAndArgument(string command)
    {
        CMD cmd;
        return cmd.validCheck(command);
    }

private:
    SSD* ssd;
    const int SSD_MINIMUM_ADDRESS = 0;
    const int SSD_MAXIMUM_ADDRESS = 100;

    bool didReadFail(string actual, string expected)
    {
        if (actual != expected)
            return true;
        return false;
    }

    bool isValidAddress(uint32_t address)
    {
        if (address > 99 || address < 0)
            return false;
        return true;
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