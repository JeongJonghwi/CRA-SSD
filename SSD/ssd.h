#ifndef __SSD_H__
#define __SSD_H__

#define OUT

#include <string>
#include <stdint.h>
#include <cstring>
#include <cstdlib>

using std::string;
using std::strtol;

#define MIN_LBA (0)
#define MAX_LBA (99)
#define SSD_NANE_FILE_NAME "ssd_nand.txt"
#define SSD_OUTPUT_FILE_NAME "ssd_output.txt"
#define SSD_ERROR_STRING "ERROR"

enum CmdType
{
    READ,
    WRITE,
};

class SSD
{
public:
    explicit SSD();
    bool Read(uint32_t nLba);
    bool Write(uint32_t nLba, uint32_t value);
    bool ReadLbaFromSsd(uint32_t nLba, uint32_t& readValue);
    bool WriteToOutputFile(uint32_t readValue);
    bool WriteToOutputFileError();
    bool isValidCheckAndCastType(int argc, char* argv[], OUT CmdType* peCmd, OUT unsigned int* pnLba, OUT unsigned int* pnValue);
private:
    uint32_t ssdBuffer[(MAX_LBA + 1)];
    bool CheckCMDandNumofParam(int argc, char* argv[], OUT CmdType* peCmd);
    bool CheckLBA(int argc, char* argv[], OUT unsigned int* pnLba);
    bool CheckValue(int argc, char* argv[], OUT unsigned int* pnValue);

};

#endif