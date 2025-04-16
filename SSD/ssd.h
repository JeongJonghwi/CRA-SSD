#ifndef __SSD_H__
#define __SSD_H__

#define MIN_LBA (0)
#define MAX_LBA (99)

#include <string>
#include <stdint.h>

using std::string;

class SSD
{
public:
    SSD();
    uint32_t Read(uint32_t nLba);
    uint32_t Write(uint32_t nLba, uint32_t value);
    static string getSsdNandFileName();
    static string getSsdOutputFileName();
private:
    static const std::string nandFileName;
    static const std::string nandOutputFileName;
};

#endif