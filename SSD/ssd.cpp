#include "ssd.h"
#include <stdexcept>
#include<io.h>
#include <fcntl.h>

using std::exception;

SSD::SSD()
{
    if (_access_s(SSD_NANE_FILE_NAME, 0) != 0) {
        // nand file is not exist
        FILE* fp = nullptr;

        if ((fopen_s(&fp, SSD_NANE_FILE_NAME, "wb+")) != 0) {
			return;
        }

        uint32_t value = 0;
        uint32_t nandBuff[MAX_LBA + 1];

		memset(nandBuff, 0x0, sizeof(uint32_t) * (MAX_LBA + 1));
		fwrite(nandBuff, sizeof(uint32_t) * (MAX_LBA + 1), 1, fp);

        fflush(fp);
        fclose(fp);
    }
}

bool SSD::Read(uint32_t nLba)
{
    uint32_t readValue = 0;

    // read operation
	if (ReadLbaFromSsd(nLba, readValue) == false) {
		return false;
	}

	if (WriteToOutputFile(readValue) == false) {
		return false;
	}

    return true;
}

bool SSD::Write(unsigned int nLba, uint32_t value)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, SSD_NANE_FILE_NAME, "wb") != 0) {
        return false;
    }

    fseek(fp, nLba * sizeof(uint32_t), SEEK_SET);

	fwrite(&value, sizeof(uint32_t), 1, fp);

    fflush(fp);
    fclose(fp);

    return true;
}

bool SSD::ReadLbaFromSsd(uint32_t nLba, uint32_t& readValue)
{
	FILE* fp = nullptr;

	if (fopen_s(&fp, SSD_NANE_FILE_NAME, "rb") != 0) {
		return false;
	}

	fseek(fp, nLba * sizeof(uint32_t), SEEK_SET);
	fread(&readValue, sizeof(uint32_t), 1, fp);

	fclose(fp);

	return true;
}

bool SSD::WriteToOutputFile(uint32_t readValue)
{
	FILE* fp = nullptr;
	// write result
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


bool SSD::isValidCheckAndCastType(int argc, char* argv[], OUT unsigned int* pnLba, OUT unsigned int* pnValue)
{
	bool bValid = true;

	bValid = bValid && CheckCMDandNumofParam(argc, argv);
	bValid = bValid && CheckLBA(argc, argv, pnLba);

	// Read
	if (argc == 3)
	{
		WriteToOutputFileError();
		return bValid;
	}

	bValid = bValid && CheckValue(argc, argv, pnValue);
	
	if (bValid == false)
	{
		WriteToOutputFileError();
	}
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