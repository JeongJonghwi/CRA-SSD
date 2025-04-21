#include "ssd.h"
#include <fcntl.h>
#include <io.h>

#define MIN_LBA (0)
#define MAX_LBA (99)
#define VALUE_INPUT_LENGTH (10)

#define READ_ARG_NUM (3)
#define WRITE_ARG_NUM (4)
#define ERASE_ARG_NUM (4)

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

bool SSD::ValidCheckAndCastType(int argc, char* argv[], OUT CmdType* cmd, OUT uint32_t* lba, OUT uint32_t* value)
{
    bool valid = true;

    valid = valid && CheckCMDandNumofParam(argc, argv, cmd);
    valid = valid && CheckLBA(argc, argv, lba);

    if (*cmd == READ) {
        return valid;
    }

    if (*cmd == WRITE) {
        valid = valid && CheckValue(argc, argv, value);
    } else {    // ERASE
        valid = valid && CheckCount(argc, argv, value);
    }

    return valid;
}

bool SSD::WriteToOutputFileError()
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_OUTPUT_FILE_NAME, "w+") != 0) {
        return false;
    }

    if (fprintf(fp, "%s", SSD_ERROR_STRING) != VALUE_INPUT_LENGTH) {
        fclose(fp);
        return false;
    }

    fflush(fp);
    fclose(fp);

    return true;
}

bool SSD::WriteToOutputFile(uint32_t readValue)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_OUTPUT_FILE_NAME, "w+") != 0) {
        return false;
    }

    if (fprintf(fp, "0x%08X", readValue) != VALUE_INPUT_LENGTH) {
        fclose(fp);
        return false;
    }

    fflush(fp);
    fclose(fp);

    return true;
}

bool SSD::Read(uint32_t lba)
{
    uint32_t readValue = 0;

    if (ReadLbaFromSsd(lba, readValue) == false) {
        return false;
    }

    if (WriteToOutputFile(readValue) == false) {
        return false;
    }

    return true;
}

bool SSD::Write(uint32_t lba, uint32_t value)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_NAND_FILE_NAME, "r+b") != 0) {
        return false;
    }

    if (fseek(fp, lba * sizeof(uint32_t), SEEK_SET) != 0) {
        fclose(fp);
        return false;
    }

    fwrite(&value, sizeof(uint32_t), 1, fp);
    fflush(fp);

    fclose(fp);

    return true;
}

bool SSD::Erase(uint32_t lba, uint32_t count)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_NAND_FILE_NAME, "r+b") != 0) {
        return false;
    }

    if (fseek(fp, lba * sizeof(uint32_t), SEEK_SET) != 0) {
        fclose(fp);
        return false;
    }

    uint32_t value[MAX_COUNT + 1];
    memset(value, 0, sizeof(uint32_t) * count);
    fwrite(value, sizeof(uint32_t), count, fp);
    fflush(fp);

    fclose(fp);

    return true;
}

CmdType SSD::GetCmdType(const string& str)
{
    CmdType ret = INVALID_CMD;

    for (int type=CMD_START; type<CMD_COUNT; type++) {
        if (cmd_symbol[type] == str) {
            ret = static_cast<CmdType>(type);
        }
    }

    return ret;
}

void SSD::InitializeNandFile() {
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

bool SSD::CheckCMDandNumofParam(int argc, char* argv[], OUT CmdType* cmd)
{
    if (argc <= 1)
        return false;

    *cmd = GetCmdType(argv[PARAM_CMD]);

    if (*cmd == READ) {
        if (argc != READ_ARG_NUM) {
            return false;
        }
    } else if (*cmd == WRITE) {
        if (argc != WRITE_ARG_NUM) {
            return false;
        }
    } else if (*cmd == ERASE) {
        if (argc != ERASE_ARG_NUM) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool SSD::CheckLBA(int argc, char* argv[], OUT uint32_t* lba)
{
    for (int i = 0; i < strlen(argv[PARAM_LBA]); i++) {
        if (argv[PARAM_LBA][i] < '0' || argv[PARAM_LBA][i] > '9') {
            return false;
        }
    }

    *lba = atoi(argv[PARAM_LBA]);
    if (*lba < MIN_LBA || *lba > MAX_LBA) {
        return false;
    }

    return true;
}

bool SSD::CheckValue(int argc, char* argv[], OUT uint32_t* value)
{
    if (strlen(argv[PARAM_VALUE]) != VALUE_INPUT_LENGTH) {
        return false;
    }
    if (argv[PARAM_VALUE][0] != '0' || argv[PARAM_VALUE][1] != 'x') {
        return false;
    }

    char* end;
    *value = (uint32_t)strtoul(argv[PARAM_VALUE], &end, 16);
    if (end != &argv[PARAM_VALUE][VALUE_INPUT_LENGTH]) {
        return false;
    }

    return true;
}

bool SSD::CheckCount(int argc, char* argv[], OUT uint32_t* count)
{
    for (int i = 0; i < strlen(argv[PARAM_VALUE]); i++) {
        if (argv[PARAM_VALUE][i] < '0' || argv[PARAM_VALUE][i] > '9') {
            return false;
        }
    }

    *count = atoi(argv[PARAM_VALUE]);
    if (*count < MIN_COUNT || *count > MAX_COUNT) {
        return false;
    }

    return true;
}

bool SSD::ReadLbaFromSsd(uint32_t lba, uint32_t& readValue)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_NAND_FILE_NAME, "rb") != 0) {
        return false;
    }

    if (fseek(fp, lba * sizeof(uint32_t), SEEK_SET) != 0) {
        fclose(fp);
        return false;
    }

    fread(&readValue, sizeof(uint32_t), 1, fp);
    fclose(fp);

    return true;
}