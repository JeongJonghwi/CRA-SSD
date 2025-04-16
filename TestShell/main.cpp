#include "gmock/gmock.h"
#include "interface.h"
#include "test_shell.cpp"

using namespace testing;

class MockSSD : public SSD {
public:
	MOCK_METHOD(string, read, (uint32_t address), (override));
	MOCK_METHOD(void, write, (uint32_t address, uint32_t value), (override));
};

TEST(ShellTest, readSuccess) {
	MockSSD ssd;
	TestShell ts{ &ssd };
	uint32_t address = 3;

	EXPECT_CALL(ssd, read(_))
		.Times(1)
		.WillRepeatedly(Return("0x10000000"));
	
	string expected = "0x10000000";
	string actual = ts.read(address);

	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, readFailWithInvalidLBA) {
	MockSSD ssd;
	TestShell ts{ &ssd };
	uint32_t address = 100;

	EXPECT_EQ("ERROR", ts.read(address));
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}