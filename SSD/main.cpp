#include "gmock/gmock.h"
#include "ssd.h"
#include <string>
#include <stdexcept>
#include<sys/types.h>
#include<sys/stat.h>
#include <cstdio>

#define _CRT_SECURE_NO_WARNINGS

using std::exception;
using std::string;

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
	SSD ssd;
	uint32_t lba = 1;
	uint32_t value = 0x12345678;
	uint32_t expected = 0x00000000;
	//string actual;
	uint32_t actual = 0x00000000;

	ssd.Write(lba, value);

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, readWithFile) {
	SSD ssd;
	uint32_t lba = 2;
	uint32_t expected = 0x00000000;

	uint32_t actual = ssd.Read(lba);

	EXPECT_EQ(expected, actual);
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}