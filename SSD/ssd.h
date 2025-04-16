#ifndef __SSD_H__
#define __SSD_H__

#define MIN_LBA (0)
#define MAX_LBA (99)

class SSD
{
public:
    unsigned int Read(unsigned int nLba);
private:
};

#endif