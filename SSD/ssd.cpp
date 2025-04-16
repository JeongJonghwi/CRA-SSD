#include "ssd.h"
#include <stdexcept>

using std::exception;

unsigned int
SSD::Read(unsigned int nLba)
{
    if (nLba < MIN_LBA || nLba > MAX_LBA)
    {
        throw std::exception("ERROR");
    }

    return 0;
}

unsigned int SSD::write(unsigned int nLba, std::string value)
{
    if (nLba < MIN_LBA || nLba > MAX_LBA)
    {
        return 1;
    }

    std::string valueHeader = value.substr(0, 2);
    std::string valueBody = value.substr(2);

    if (valueHeader != "0x" && valueHeader != "0X")
        return 1;

    for (auto str : valueBody) {
        if (isdigit(str)) continue;
        if (str >= 'A' && str <= 'F') continue;
        if (str >= 'a' && str <= 'f') continue;
        return 1;
    }

    return 0;
}