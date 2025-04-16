#include "gmock/gmock.h"
#include "ssd.h"
#include <string>
#include <stdexcept>

using std::exception;
using std::string;

TEST(SSDTest, readSuccess) {
	SSD ssd;
	const uint32_t VALID_LBA = 0;
	uint32_t expected = 0x00000000;

	uint32_t actual = ssd.Read(VALID_LBA);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, readFailWithInvalidLBA) {
	SSD ssd;
	const uint32_t INVALID_LBA = 100;

	try {
		ssd.Read(INVALID_LBA);
		FAIL();
	}
	catch (exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "ERROR" });
	}
}

TEST(SSDTest, writeSuccess) {
	SSD ssd;
	uint32_t lba = 1;
	string value = "0x12345678";

	int expected = 0;

	EXPECT_EQ(expected, ssd.write(lba, value));
}

TEST(SSDTest, writeFailWithInvalidLBA) {
	SSD ssd;
	uint32_t lba = 100;
	string value{ "0x12345678" };

	int expected = 1;

	EXPECT_EQ(expected, ssd.write(lba, value));
}

TEST(SSDTest, writeFailWithInvalidValue) {
	SSD ssd;
	uint32_t lba = 1;
	string value{ "0xTTTTFFFF" };

	int expected = 1;

	EXPECT_EQ(expected, ssd.write(lba, value));
}

TEST(SSDTest, writeWithFile) {
	SSD ssd;
	uint32_t lba = 1;
	string value{ "0x12345678" };
	string expected{ "0x00000000" };
	string actual;

	ssd.write(lba, value);

	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, readWithFile) {
	SSD ssd;
	uint32_t lba = 1;
	string expected{ "0x00000000" };
	string actual;

	ssd.Read(lba);

	FILE *f;
	f = open("ssd_nand.txt", "r");
	read(f, &actual, 4);

	EXPECT_EQ(expected, actual);
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}