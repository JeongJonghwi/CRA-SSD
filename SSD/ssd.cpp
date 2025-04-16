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