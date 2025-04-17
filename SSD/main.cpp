#include "gmock/gmock.h"
#include "ssd.h"
#include <iostream>
#include <string>
#include <stdexcept>

#include<sys/types.h>
#include<sys/stat.h>
#include <cstdio>
#include <filesystem>
#include <cstring>

#define _CRT_SECURE_NO_WARNINGS

using std::exception;
using std::string;

class SSDTestFixture : public ::testing::Test {
public:
	SSD ssd;
	char strReadValue[20];

	string getSsdLbaValue(uint32_t nLba) {
		int result = 0;
		FILE* fp = nullptr;

		if ((result = fopen_s(&fp, SSD_OUTPUT_FILE_NAME, "r")) != 0) {
			fopen_s(&fp, SSD_OUTPUT_FILE_NAME, "r"); // try to open again when fail
		}

		fseek(fp, 0, SEEK_SET);
		//fscanf_s(fp, "%s", buf, 10);
		fread(strReadValue, sizeof(char) * 10, 1, fp);
		strReadValue[10] = '\0';

		fclose(fp);

		return string(strReadValue);
	}
protected:
	void TearDown() override {
		remove(SSD_NANE_FILE_NAME);
		remove(SSD_OUTPUT_FILE_NAME);
	}
};

TEST_F(SSDTestFixture, readSuccess) {
	const uint32_t VALID_LBA = 0;
	string expected{ "0x00000000" };

	ssd.Read(VALID_LBA);
	string actual = getSsdLbaValue(VALID_LBA);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, writeSuccess) {
	uint32_t lba = 1;
	uint32_t value = 0x12345678;

	string expected{ "0x12345678" };

	ssd.Write(lba, value);
	ssd.Read(lba);
	string actual = getSsdLbaValue(lba);

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, writeWithFile) {
	uint32_t lba = 1;
	uint32_t value = 0x12345678;
	string expected{ "0x12345678" };

	ssd.Write(lba, value);
	ssd.Read(lba);

	string actual = getSsdLbaValue(lba);

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, readWithFile) {
	uint32_t lba = 1;
	string expected{ "0x00000000" };

	ssd.Read(lba);
	string actual = getSsdLbaValue(lba);

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, checkArgumentRead) {
	int argc = 3;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("R");
	argv[2] = const_cast<char*>("1");

	bool expected = true;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
	EXPECT_EQ(1, lba);
}

TEST_F(SSDTestFixture, checkArgumentWrite) {
	int argc = 4;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("W");
	argv[2] = const_cast<char*>("1");
	argv[3] = const_cast<char*>("0x12345678");

	bool expected = true;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
	EXPECT_EQ(1, lba);
	EXPECT_EQ(0x12345678, value);
}

TEST_F(SSDTestFixture, invalidArgumentsCountTest1) {
	int argc = 1;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	bool expected = false;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, invalidArgumentsCountTest2) {
	int argc = 3;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("W"); // write need totally 4 arguments

	bool expected = false;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, invalidArgumentsCountTest3) {
	int argc = 4;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("R"); // read need totally 3 arguments

	bool expected = false;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, invalidOperationTest) {
	int argc = 3;
	char* argv[3];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("C"); // not R or W

	bool expected = false;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, invalidAddressRangeTest) {
	int argc = 2;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("R");
	argv[2] = const_cast<char*>("100"); // LBA Range must in 0-99

	bool expected = false;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, invalidValueTest) {
	int argc = 4;
	char* argv[4];
	uint32_t lba;
	uint32_t value;

	argv[1] = const_cast<char*>("W");
	argv[2] = const_cast<char*>("50");
	argv[3] = const_cast<char*>("0xTTTTFFFF"); // not hexadeciamal

	bool expected = false;
	bool actual = ssd.isValidCheckAndCastType(argc, argv, &lba, &value);
	EXPECT_EQ(expected, actual);
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}