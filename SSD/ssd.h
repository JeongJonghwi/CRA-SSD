#pragma once

#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <string>

using std::string;

#define OUT

#define SSD_NAND_FILE_NAME "ssd_nand.txt"
#define SSD_OUTPUT_FILE_NAME "ssd_output.txt"
#define SSD_ERROR_STRING "ERROR"
#define MIN_COUNT (0)
#define MAX_COUNT (10)

enum CmdType {
    CMD_START = 0,
    READ = CMD_START,
    WRITE,
    ERASE,
    FLUSH,
    CMD_COUNT,
    INVALID_CMD = 0xFF
};

const string cmd_symbol[CMD_COUNT] = { "R", "W", "E", "F" };

class SSD {
public:
    static SSD& GetInstance();

    bool ValidCheckAndCastType(int argc, char* argv[], OUT CmdType* cmd, OUT uint32_t* lba, OUT uint32_t* value);
    void InitializeNandFile();
    bool WriteToOutputFileError();
    bool WriteToOutputFile(uint32_t readValue);
    bool Read(uint32_t lba);
    bool Write(uint32_t lba, uint32_t value);
    bool Erase(uint32_t lba, uint32_t count);

    CmdType GetCmdType(const string& str);

private:
    explicit SSD();
    SSD(const SSD& ssd) = delete;
    SSD& operator=(const SSD& ssd) = delete;

    bool CheckCMDandNumofParam(int argc, char* argv[], OUT CmdType* cmd);
    bool CheckLBA(int argc, char* argv[], OUT uint32_t* lba);
    bool CheckValue(int argc, char* argv[], OUT uint32_t* value);
    bool CheckCount(int argc, char* argv[], uint32_t lba, OUT uint32_t* count);

    bool ReadLbaFromSsd(uint32_t lba, uint32_t& readValue);
};