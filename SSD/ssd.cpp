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
    char buf[32];
    FILE* fp = nullptr;

    // read operation
    if ((result = fopen_s(&fp, nandFileName.c_str(), "rb")) != 0) {
        fopen_s(&fp, nandFileName.c_str(), "rb"); // try to open again when fail
    }

    fseek(fp, nLba * sizeof(uint32_t), SEEK_SET);
    fread(&readValue, sizeof(uint32_t), 1, fp);

    fclose(fp);

    // write result
    if ((result = fopen_s(&fp, nandOutputFileName.c_str(), "w+")) != 0) {
        fopen_s(&fp, nandOutputFileName.c_str(), "w+");
    }

    fprintf(fp, "0x%08x", readValue);

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

    fseek(fp, nLba * sizeof(uint32_t), SEEK_SET);

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

bool SSD::isValidCheckAndCastType(int argc, char* argv[], OUT unsigned int* pnLba, OUT unsigned int* pnValue)
{
	bool bValid = true;

	bValid = bValid && CheckCMDandNumofParam(argc, argv);
	bValid = bValid && CheckLBA(argc, argv, pnLba);

	// Read
	if (argc == 3)
	{
		return bValid;
	}

	bValid = bValid && CheckValue(argc, argv, pnValue);
	return bValid;
}

bool SSD::CheckCMDandNumofParam(int argc, char* argv[])
{
	if (argc < 2)
	{
		return false;
	}
	if (strcmp(argv[1], "R") == 0)
	{
		if (argc != 3)
		{
			return false;
		}
	}
	else if (strcmp(argv[1], "W") == 0)
	{
		if (argc != 4)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

bool SSD::CheckLBA(int argc, char* argv[], OUT unsigned int* pnLba)
{
	for (int i=0; i<strlen(argv[2]); i++)
	{
		if (argv[2][i] < '0' || argv[2][i] > '9')
		{
			return false;
		}
	}

	*pnLba = atoi(argv[2]);
	if (*pnLba < MIN_LBA || *pnLba > MAX_LBA)
	{
		return false;
	}

	return true;
}

bool SSD::CheckValue(int argc, char* argv[], OUT unsigned int* pnValue)
{
	if (strlen(argv[3]) != VALUE_INPUT_LENGTH)
	{
		return false;
	}
	if (argv[3][0] != '0' || argv[3][1] != 'x')
	{
		return false;
	}

	char *end;
	*pnValue = (unsigned int)strtol(argv[3], &end, 16);
	if (end != &argv[3][VALUE_INPUT_LENGTH])
	{
		return false;
	}

	return true;
}