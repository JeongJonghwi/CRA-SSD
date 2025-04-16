#include "gmock/gmock.h"
#include "interface.h"
#include "test_shell.cpp"

using namespace testing;

const int VALID_ADDRESS = 3;
const int INVALID_ADDRESS = 100;
const string VALID_VALUE = "0x12345678";
const string INVALID_VALUE = "0xTTTTFFFF";

class MockSSD : public SSD {
public:
	MOCK_METHOD(string, read, (uint32_t address), (override));
	MOCK_METHOD(void, write, (uint32_t address, string value), (override));
};

TEST(ShellTest, readSuccess) {
	MockSSD ssd;
	TestShell ts{ &ssd };
	uint32_t address = VALID_ADDRESS;

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
	uint32_t address = INVALID_ADDRESS;

	EXPECT_EQ("ERROR", ts.read(address));
}

TEST(ShellTest, writeSuccess) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	EXPECT_CALL(ssd, write(_, _))
		.Times(1);

	uint32_t address = VALID_ADDRESS;
	string value = VALID_VALUE;
	string expected = "Done";
	string actual = ts.write(address, value);
	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, writeFailWithInvalidLBA) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	EXPECT_CALL(ssd, write(_, _))
		.Times(0);

	uint32_t address = INVALID_ADDRESS;
	string value = VALID_VALUE;
	string expected = "ERROR";
	string actual = ts.write(address, value);
	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, writeFailWithInvalidValue) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	EXPECT_CALL(ssd, write(_, _))
		.Times(0);

	uint32_t address = VALID_ADDRESS;
	string value = INVALID_VALUE;
	string expected = "ERROR";
	string actual = ts.write(address, value);
	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, helpTest) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	string expected = "Team CodeCraft: ";
	expected += "°­µ¿Çö, ±èÅÂ¿Ï, À¯Çö½Â, ÀÌ¼öÁö, Á¤Á¾ÈÖ\n";
	expected += "write: write {value} to {address}\n";
	expected += "read: read from {address}\n";
	expected += "fullwrite: write {value} to 0x0 ~ 0x100\n";
	expected += "fullread: read from all 0x0 ~ 0x100\n";
	expected += "help: print this message\n";
	expected += "exit: exit TestShell\n";
	string actual = ts.help();

	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, exitTest) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	string expected = "Exit TestShell";
	string actual = ts.exit();

	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, fullWriteTest) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	EXPECT_CALL(ssd, write(_, _))
		.Times(100);

	string value = VALID_VALUE;

	ts.fullWrite(value);
}

TEST(ShellTest, fullReadTest) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	EXPECT_CALL(ssd, read(_))
		.Times(100)
		.WillRepeatedly(Return("0x00000000"));

	ts.fullRead();
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}