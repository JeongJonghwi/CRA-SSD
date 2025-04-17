#include "ssd.h"
#include <fcntl.h>
#include <io.h>

#define MIN_LBA (0)
#define MAX_LBA (99)
#define VALUE_INPUT_LENGTH (10)

#define READ_ARG_NUM (3)
#define WRITE_ARG_NUM (4)

#define PARAM_CMD (1)
#define PARAM_LBA (2)
#define PARAM_VALUE (3)

SSD::SSD()
{
    if (_access_s(SSD_NAND_FILE_NAME, 0) == 0)
        return;

    FILE* fp = nullptr;

    if ((fopen_s(&fp, SSD_NAND_FILE_NAME, "wb+")) != 0) {
        exit(0);
    }

    uint32_t nandBuff[MAX_LBA + 1];

    memset(nandBuff, 0x0, sizeof(uint32_t) * (MAX_LBA + 1));
    fwrite(nandBuff, sizeof(uint32_t) * (MAX_LBA + 1), 1, fp);

    fflush(fp);
    fclose(fp);
}

bool SSD::Read(uint32_t nLba)
{
    uint32_t readValue = 0;

    if (ReadLbaFromSsd(nLba, readValue) == false) {
        return false;
    }

    if (WriteToOutputFile(readValue) == false) {
        return false;
    }

    return true;
}

bool SSD::Write(uint32_t nLba, uint32_t value)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_NAND_FILE_NAME, "r+b") != 0) {
        return false;
    }

    if (fseek(fp, nLba * sizeof(uint32_t), SEEK_SET) != 0) {
        return false;
    }

    fwrite(&value, sizeof(uint32_t), 1, fp);
    fflush(fp);

    fclose(fp);

    return true;
}

bool SSD::ReadLbaFromSsd(uint32_t nLba, uint32_t& readValue)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_NAND_FILE_NAME, "rb") != 0) {
        return false;
    }

    if (fseek(fp, nLba * sizeof(uint32_t), SEEK_SET) != 0) {
        return false;
    }

    fread(&readValue, sizeof(uint32_t), 1, fp);
    fclose(fp);

    return true;
}

bool SSD::WriteToOutputFile(uint32_t readValue)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_OUTPUT_FILE_NAME, "w+") != 0) {
        return false;
    }

    if (fprintf(fp, "0x%08x", readValue) != VALUE_INPUT_LENGTH) {
        return false;
    }

    fflush(fp);
    fclose(fp);

    return true;
}

bool SSD::WriteToOutputFileError()
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_OUTPUT_FILE_NAME, "w+") != 0) {
        return false;
    }

    if (fprintf(fp, "%s", SSD_ERROR_STRING) != VALUE_INPUT_LENGTH) {
        return false;
    }

    fflush(fp);
    fclose(fp);

    return true;
}

bool SSD::isValidCheckAndCastType(int argc, char* argv[], OUT CmdType* peCmd, OUT unsigned int* pnLba, OUT unsigned int* pnValue)
{
    bool bValid = true;

    bValid = bValid && CheckCMDandNumofParam(argc, argv, peCmd);
    bValid = bValid && CheckLBA(argc, argv, pnLba);

    if (argc == READ_ARG_NUM) {
        return bValid;
    }

    bValid = bValid && CheckValue(argc, argv, pnValue);

    return bValid;
}

bool SSD::CheckCMDandNumofParam(int argc, char* argv[], OUT CmdType* peCmd)
{
    if (argc <= 1) return false;

    if (strcmp(argv[PARAM_CMD], "R") == 0) {
        if (argc != READ_ARG_NUM) {
            return false;
        }
        *peCmd = READ;
    } else if (strcmp(argv[PARAM_CMD], "W") == 0) {
        if (argc != WRITE_ARG_NUM) {
            return false;
        }
        *peCmd = WRITE;
    } else {
        return false;
    }

    return true;
}

bool SSD::CheckLBA(int argc, char* argv[], OUT unsigned int* pnLba)
{
    for (int i = 0; i < strlen(argv[PARAM_LBA]); i++) {
        if (argv[PARAM_LBA][i] < '0' || argv[PARAM_LBA][i] > '9') {
            return false;
        }
    }

    *pnLba = atoi(argv[PARAM_LBA]);
    if (*pnLba < MIN_LBA || *pnLba > MAX_LBA) {
        return false;
    }

    return true;
}

bool SSD::CheckValue(int argc, char* argv[], OUT unsigned int* pnValue)
{
    if (strlen(argv[PARAM_VALUE]) != VALUE_INPUT_LENGTH) {
        return false;
    }
    if (argv[PARAM_VALUE][0] != '0' || argv[PARAM_VALUE][1] != 'x') {
        return false;
    }

    char* end;
    *pnValue = (unsigned int)strtoul(argv[PARAM_VALUE], &end, 16);
    if (end != &argv[PARAM_VALUE][VALUE_INPUT_LENGTH]) {
        return false;
    }

    return true;
}