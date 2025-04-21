#include "cmd.h"

#define MIN_LBA_NUM 0
#define MAX_LBA_NUM 99

bool CMD::validCheck(string command)
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
    return false;
}

int CMD::lbaLimitRange(int i)
{
    if (i < MIN_LBA_NUM)
        return MIN_LBA_NUM;
    if (i > MAX_LBA_NUM)
        return MAX_LBA_NUM;
    return i;
}

void CMD::setValidEraseSize(int s)
{
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
    logger.Print("CMD.setValidEraseSize()", "address : " + std::to_string(address) + " size : " + std::to_string(size));
}

void CMD::setValidEraseRangeSize(int startLba, int endLba)
{
    if (startLba > endLba) {
        address = endLba;
        endLba = startLba;
        startLba = address;
    }
    size = endLba - startLba + 1;
    logger.Print("CMD.setValidEraseRangeSize()", "address : " + std::to_string(address) + " size : " + std::to_string(size));
}

bool CMD::isValidLBA(string lba)
{
    if (!isNumber(lba)) {
        logger.Print("CMD.isValidLBA()", "LBA is not Number : " + lba);
        return false;
    }
    if (!isValidAddress(stoi(lba))) {
        logger.Print("CMD.isValidLBA()", "LBA is not in range : " + lba);
        return false;
    }
    return true;
}

bool CMD::isValidAddress(uint32_t address)
{
    if (address > MAX_LBA_NUM || address < 0)
        return false;
    return true;
}

bool CMD::isValidValue(string value)
{
    if (value.length() != 10 || value[0] != '0' || value[1] != 'x') {
        logger.Print("CMD.isValidValue()", "Value is not valid length & format : " + value);
        return false;
    }

    for (int i = 2; i < 10; i++) {
        if ((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F') || (value[i] >= 'a' && value[i] <= 'f'))
            continue;
        logger.Print("CMD.isValidValue()", "Value is not HEX : " + value);
        return false;
    }
    return true;
}

vector<string> CMD::splitBySpace(const string& input)
{
    std::istringstream iss(input);
    std::string word;
    std::vector<string> result;

    while (iss >> word) {
        result.push_back(word);
    }
    return result;
}

bool CMD::isNumber(string str)
{
    if (str.size() > 2)
        return false;

    for (auto charactor : str) {
        if (charactor < '0' || charactor > '9')
            return false;
    }
    return true;
}