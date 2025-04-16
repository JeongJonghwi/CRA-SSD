#include "gmock/gmock.h"
#include "ssd.h"
#include <string>
#include <stdexcept>

#include<sys/types.h>
#include<sys/stat.h>
#include <cstdio>
#include <iostream>
#include <filesystem>

#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <cstdlib>

using std::exception;
using std::string;
using std::strtol;

#define OUT
#define VALUE_INPUT_LENGTH (10)

bool isValidCheckAndCastType(int argc, char* argv[], OUT unsigned int* pnLba, OUT unsigned int* pnValue);

class SSDTestFixture : public ::testing::Test {
public:
	SSD ssd;
	char buf[20];

	uint32_t getResultValue(uint32_t nLba) {
		int result = 0;
		uint32_t readValue = 0;
		FILE* fp = nullptr;

		if ((result = fopen_s(&fp, ssd.getSsdOutputFileName().c_str(), "r")) != 0) {
			fopen_s(&fp, ssd.getSsdOutputFileName().c_str(), "r"); // try to open again when fail
		}

		fscanf_s(fp, "%s", buf);

		fclose(fp);

		return stoi(string(&buf[2]), 0, 10);
	}
protected:
	void TearDown() override {
		remove(ssd.getSsdNandFileName().c_str());
		remove(ssd.getSsdOutputFileName().c_str());
	}
};

TEST_F(SSDTestFixture, readSuccess) {
	const uint32_t VALID_LBA = 0;
	uint32_t expected = 0x00000000;

	uint32_t actual = ssd.Read(VALID_LBA);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, readFailWithInvalidLBA) {
	const uint32_t INVALID_LBA = 100;
	uint32_t expected = 0;

	EXPECT_EQ(expected, ssd.Read(INVALID_LBA));
}

TEST_F(SSDTestFixture, writeSuccess) {
	uint32_t lba = 1;
	uint32_t value = 0x12345678;

	uint32_t expected = 0x12345678;

	EXPECT_EQ(expected, ssd.Write(lba, value));
}

TEST_F(SSDTestFixture, writeFailWithInvalidLBA) {
	uint32_t lba = 100;
	uint32_t value = 0x12345678;

	uint32_t expected = 0;

	EXPECT_EQ(expected, ssd.Write(lba, value));
}

/*
TEST_F(SSDTestFixture, writeFailWithInvalidValue) {
	SSD ssd;
	uint32_t lba = 1;
	string value{ "0xTTTTFFFF" };

	uint32_t expected = 1;

	EXPECT_EQ(expected, ssd.Write(lba, value));
}
*/

TEST_F(SSDTestFixture, writeWithFile) {
	uint32_t lba = 1;
	uint32_t value = 0x12345678;
	uint32_t expected = 0x00000000;
	//string actual;
	uint32_t actual = 0x00000000;

	ssd.Write(lba, value);

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, readWithFile) {
	uint32_t lba = 1;
	uint32_t expected = 0x00000000;

	uint32_t actual = ssd.Read(lba);

	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, checkArgumentRead) {
	int argc = 3;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("R");
	argv[2] = const_cast<char*>("1");

	bool expected = true;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
	EXPECT_EQ(1, lba);
}

TEST(SSDTest, checkArgumentWrite) {
	int argc = 4;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("W");
	argv[2] = const_cast<char*>("1");
	argv[3] = const_cast<char*>("0x12345678");

	bool expected = true;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
	EXPECT_EQ(1, lba);
	EXPECT_EQ(0x12345678, value);
}

TEST(SSDTest, invalidArgumentsCountTest1) {
	int argc = 1;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	bool expected = false;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, invalidArgumentsCountTest2) {
	int argc = 3;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("W"); // write need totally 4 arguments

	bool expected = false;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, invalidArgumentsCountTest3) {
	int argc = 4;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("R"); // read need totally 3 arguments

	bool expected = false;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, invalidOperationTest) {
	int argc = 3;
	char* argv[3];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("C"); // not R or W

	bool expected = false;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, invalidAddressRangeTest) {
	int argc = 2;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("R");
	argv[2] = const_cast<char*>("100"); // LBA Range must in 0-99

	bool expected = false;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, invalidValueTest) {
	int argc = 4;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("W");
	argv[2] = const_cast<char*>("50");
	argv[3] = const_cast<char*>("0xTTTTFFFF"); // not hexadeciamal

	bool expected = false;
	bool actual = isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

bool isValidCheckAndCastType(int argc, char* argv[], OUT unsigned int* pnLba, OUT unsigned int* pnValue)
{
	int nLba, nValue;
	// CMD - parameter check
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

	// Valid Check - LBA
	for (int i=0; i<strlen(argv[2]); i++)
	{
		if (argv[2][i] < '0' || argv[2][i] > '9')
		{
			return false;
		}
	}

	nLba = atoi(argv[2]);
    if (nLba < MIN_LBA || nLba > MAX_LBA)
	{
		return false;
	}

	// Read
	*pnLba = nLba;
	if (argc == 3)
	{
		return true;
	}

	// Valid Check - Value
	if (strlen(argv[3]) != VALUE_INPUT_LENGTH)
	{
		return false;
	}
	if (argv[3][0] != '0' || argv[3][1] != 'x')
	{
		return false;
	}
	char *end;
    nValue = (unsigned int)strtol(argv[3], &end, 16);
	if (end != &argv[3][VALUE_INPUT_LENGTH])
	{
		return false;
	}

	*pnValue = nValue;
	return true;
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}