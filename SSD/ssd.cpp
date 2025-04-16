#include "ssd.h"
#include <stdexcept>
#include<io.h>
#include <fcntl.h>

using std::exception;

const std::string SSD::nandFileName = "ssd_nand.txt";
const std::string SSD::nandOutputFileName = "ssd_output.txt";

SSD::SSD()
{
    int fileExist;
    if ((fileExist = _access_s(nandFileName.c_str(), 0)) != 0) {
        // nand file is not exist
        FILE* fp = nullptr;
        int returnResult = 0;

        if ((returnResult = fopen_s(&fp, nandFileName.c_str(), "wb+")) != 0) {
            fopen_s(&fp, nandFileName.c_str(), "wb+"); // try to open again when fail
        }

        uint32_t value = 0;
        // need to optimize
        for (int i = 0; i <= MAX_LBA; ++i) {
            fwrite(&value, sizeof(uint32_t), 1, fp);
        }

        fflush(fp);
        fclose(fp);
    }
}

uint32_t SSD::Read(uint32_t nLba)
{
    if (nLba < MIN_LBA || nLba > MAX_LBA)
    {
        return 0;
    }

    int result = 0;
    uint32_t readValue = 0;
    FILE* fp = nullptr;

    // read operation
    if ((result = fopen_s(&fp, nandFileName.c_str(), "rb")) != 0) {
        fopen_s(&fp, nandFileName.c_str(), "rb"); // try to open again when fail
    }

    fseek(fp, nLba, SEEK_SET);

    fread(&readValue, sizeof(uint32_t), 1, fp);
    fclose(fp);

    // write result
    if ((result = fopen_s(&fp, nandOutputFileName.c_str(), "w+")) != 0) {
        fopen_s(&fp, nandOutputFileName.c_str(), "w+");
    }

    fprintf(fp, "0x%x", readValue);

    fflush(fp);
    fclose(fp);

    return readValue;
}

uint32_t SSD::Write(unsigned int nLba, uint32_t value)
{
    if (nLba < MIN_LBA || nLba > MAX_LBA)
    {
        return 0;
    }

    int result = 0;
    FILE* fp = nullptr;

    if ((result = fopen_s(&fp, nandFileName.c_str(), "wb")) != 0) {
        fopen_s(&fp, nandFileName.c_str(), "wb");
    }

    fseek(fp, nLba, SEEK_SET);

    fwrite(&value, sizeof(uint32_t), 1, fp);

    fflush(fp);
    fclose(fp);

    return value;
}

std::string SSD::getSsdNandFileName()
{
    return nandFileName;
}

std::string SSD::getSsdOutputFileName()
{
    return nandOutputFileName;
}