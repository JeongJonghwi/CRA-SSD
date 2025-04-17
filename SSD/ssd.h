#ifndef __SSD_H__
#define __SSD_H__

#define MIN_LBA (0)
#define MAX_LBA (99)
#define VALUE_INPUT_LENGTH (10)
#define OUT

#include <string>
#include <stdint.h>
#include <cstring>
#include <cstdlib>

using std::string;
using std::strtol;

class SSD
{
public:
    SSD();
    uint32_t Read(uint32_t nLba);
    uint32_t Write(uint32_t nLba, uint32_t value);
    static string getSsdNandFileName();
    static string getSsdOutputFileName();
    static bool isValidCheckAndCastType(int argc, char* argv[], OUT unsigned int* pnLba, OUT unsigned int* pnValue);
private:
    static bool CheckCMDandNumofParam(int argc, char* argv[]);
    static bool CheckLBA(int argc, char* argv[], OUT unsigned int* pnLba);
    static bool CheckValue(int argc, char* argv[], OUT unsigned int* pnValue);

    static const std::string nandFileName;
    static const std::string nandOutputFileName;
};

#endif