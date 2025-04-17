#pragma once

#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <string>

#define OUT

using std::string;

#define SSD_NAND_FILE_NAME "ssd_nand.txt"
#define SSD_OUTPUT_FILE_NAME "ssd_output.txt"
#define SSD_ERROR_STRING "ERROR"

enum CmdType {
    READ,
    WRITE,
};

class SSD {
public:
    explicit SSD();
    SSD(const SSD& ssd) = delete;
    SSD& operator=(const SSD& ssd) = delete;

    bool Read(uint32_t nLba);
    bool Write(uint32_t nLba, uint32_t value);
    bool ReadLbaFromSsd(uint32_t nLba, uint32_t& readValue);
    bool WriteToOutputFile(uint32_t readValue);
    bool WriteToOutputFileError();
    bool isValidCheckAndCastType(int argc, char* argv[], OUT CmdType* peCmd, OUT unsigned int* pnLba, OUT unsigned int* pnValue);

private:
    bool CheckCMDandNumofParam(int argc, char* argv[], OUT CmdType* peCmd);
    bool CheckLBA(int argc, char* argv[], OUT unsigned int* pnLba);
    bool CheckValue(int argc, char* argv[], OUT unsigned int* pnValue);
};