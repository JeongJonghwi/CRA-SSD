#include "gmock/gmock.h"

TEST(SSDTest, readSuccess) {
	SSD ssd;
	uint32_t lba = 0;
	uint32_t expected = 0x00000000;

	uint32_t actual = ssd.read(lba);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, readFailWithInvalidLBA) {
	SSD ssd;
	uint32_t lba = 100;

	try {
		ssd.read(lba);
		FAIL();
	}
	catch (exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "Out of LBA range" });
	}
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}