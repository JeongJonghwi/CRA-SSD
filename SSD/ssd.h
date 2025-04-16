#ifndef __SSD_H__
#define __SSD_H__

#define MIN_LBA (0)
#define MAX_LBA (99)

#include <string>

using std::string;

class SSD
{
public:
    unsigned int Read(unsigned int nLba);
    unsigned int write(unsigned int nLba, string value);
private:
};

#endif