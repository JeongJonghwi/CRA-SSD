#pragma once
#include "cmd.cpp"
#include "ssd_interface.h"
#include "gmock/gmock.h"
#include <string>
#include <vector>

#include "Logger.h"

class TestShell {
public:
    TestShell(SSD* ssd)
        : ssd(ssd)
    {
    }
    virtual ~TestShell() { logger.CloseLogger(); }
    string read(uint32_t address);
    string write(uint32_t address, string value);
    string help();
    string exit();
    string fullWrite(string value);
    string fullRead();
    string erase(int address, int size);
    bool isValidCommandAndArgument(string command);

private:
    SSD* ssd;
    Logger& logger = Logger::getInstance();
    const int SSD_MINIMUM_ADDRESS = 0;
    const int SSD_MAXIMUM_ADDRESS = 100;

    bool isValidAddress(uint32_t address);
    bool isValidValue(string value);
};