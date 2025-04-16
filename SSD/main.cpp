#include "gmock/gmock.h"
#include "ssd.h"
#include <string>
#include <stdexcept>

using std::exception;
using std::string;

TEST(SSDTest, readSuccess) {
	SSD ssd;
	uint32_t lba = 0;
	uint32_t expected = 0x00000000;

	uint32_t actual = ssd.Read(lba);
	EXPECT_EQ(expected, actual);
}

TEST(SSDTest, readFailWithInvalidLBA) {
	SSD ssd;
	uint32_t lba = 100;

	try {
		ssd.Read(lba);
		FAIL();
	}
	catch (exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "ERROR" });
	}
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}