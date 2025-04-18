#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

class CMD {
public:
    bool validCheck(string command)
    {
        vector<string> rawdata = splitBySpace(command);

        if (rawdata.size() <= 0 || rawdata.size() > 3)
            return false;

        input = rawdata[0];
        if (rawdata.size() == 3) {
            if (input != "write" && input != "erase" && input != "erase_range") {
                address = -1;
                return false;
            }

            if (input == "write") {
                if (!isValidLBA(rawdata[1]))
                    return false;
                address = stoi(rawdata[1]);
                if (!isValidValue(rawdata[2]))
                    return false;
                data = rawdata[2];
            }
            
            if (input == "erase") {
                if (!isValidLBA(rawdata[1]))
                    return false;
                address = stoi(rawdata[1]);

                setValidEraseSize(stoi(rawdata[2]));
            }

            if (input == "erase_range") {
                if (!isValidLBA(rawdata[1]))
                    return false;
                address = stoi(rawdata[1]);
                if (!isValidLBA(rawdata[2]))
                    return false;
                setValidEraseRangeSize(stoi(rawdata[1]), stoi(rawdata[2]));
            }

            return true;
        }

        if (rawdata.size() == 2) {
            if (input != "read" && input != "fullwrite")
                return false;

            if (input == "read") {
                if (!isValidLBA(rawdata[1]))
                    return false;
                address = stoi(rawdata[1]);

                return true;
            }

            if (!isValidValue(rawdata[1]))
                return false;
            data = rawdata[1];
            return true;
        }

        if (input == "help" || input == "exit" || input == "fullread" || input == "flush")
            return true;

        if (isTestScript(input))
            return true;

        return false;
    }

    string getCommand()
    {
        return input;
    }

    int getAddress()
    {
        return address;
    }

    string getValue()
    {
        return data;
    }

    int getSize() {
        return size;
    }

private:
    string input;
    int address;
    string data;
    int size;

    int lbaLimitRange(int i) {
        if (i < 0)
            return 0;
        if (i > 99)
            return 99;
        return i;
    }

    void setValidEraseSize(int s) {
        int startLba;
        int endLba;
        size = 0;
        if (s == 0) {
            size = 0;
            return;
        }

        if (s < 0) {
            startLba = address + s + 1;
            endLba = address;
        } else if (s > 0) {
            startLba = address;
            endLba = address + s - 1;
        }
        startLba = lbaLimitRange(startLba);
        endLba = lbaLimitRange(endLba);

        address = startLba;
        size = endLba - startLba + 1;
    }
    
    void setValidEraseRangeSize(int startLba, int endLba) {
        if (startLba > endLba) {
            address = endLba;
            endLba = startLba;
            startLba = address;
        }
        size = endLba - startLba + 1;
    }

    bool isValidLBA(string lba) {
        if (!isNumber(lba))
            return false;
        if (!isValidAddress(stoi(lba)))
            return false;
        return true;
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

    vector<string> splitBySpace(const string& input)
    {
        std::istringstream iss(input);
        std::string word;
        std::vector<string> result;

        while (iss >> word) {
            result.push_back(word);
        }
        return result;
    }

    bool isNumber(string s)
    {
        if (s.size() > 2)
            return false;

        for (auto c : s) {
            if (c < '0' || c > '9')
                return false;
        }
        return true;
    }

    bool isTestScript(string s)
    {
        if (s == "1_" || s == "2_" || s == "3_")
            return true;

        if (s == "1_FullWriteAndReadCompare"
            || s == "2_PartialLBAWrite"
            || s == "3_WriteReadAging")
            return true;

        return false;
    }
};