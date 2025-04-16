#include "gmock/gmock.h"

#define interface struct
interface SSD {
public:
	virtual uint32_t read(uint32_t address) = 0;
	virtual void write(uint32_t address, uint32_t value) = 0;
};

class MockSSD : public testing::Test {
public:
	MOCK_METHOD(uint32_t, read, (uint32_t address), (override));
	MOCK_METHOD(void, write, (uint32_t address, uint32_t value), (override));
};

TEST(ShellTest, readSuccess) {
	MockSSD ssd;
	TestShell ts{ &ssd };
	uint32_t address;

	EXPECT_CALL(ssd, read(_))
		.Times(1)
		.WillRepeatedly(Return(0x00000000));

	uint32_t expected = 0x00000000;
	uint32_t actual = ts.read();

	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, readFailWithInvalidLBA) {
	TestShell ts;
	MockSSD ssd;
	uint32_t address;

	try {
		ts.read(address);
		FAIL();
	}
	catch (exception& e) {
		EXPECT_EQ(string{ e.what() }, string{ "Out of LBA Range" });
	}
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}