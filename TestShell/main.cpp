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
	expected += "������, ���¿�, ������, �̼���, ������\n";
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

TEST(ShellTest, groupWriteAndReadCompare) {
	MockSSD ssd;
	TestShell ts{ &ssd };

	EXPECT_CALL(ssd, write(_,_)).Times(5);
	EXPECT_CALL(ssd, read(_)).Times(5).WillRepeatedly(Return(VALID_VALUE));
	
	string expected = "PASS";
	string actual = ts.groupWriteAndReadCompare(5, 10, VALID_VALUE);
	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, partialLBAWriteTest) {
	MockSSD ssd;
	TestShell ts{ &ssd };
	string value = VALID_VALUE;

	EXPECT_CALL(ssd, write(_, _))
		.Times(150);

	EXPECT_CALL(ssd, read(_))
		.Times(150)
		.WillRepeatedly(Return(VALID_VALUE));

	string expected = "PASS";
	string actual = ts.partialLBAWrite(value);
	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, writeReadAging) {
	MockSSD ssd;
	TestShell ts{ &ssd };
	string value = VALID_VALUE;
	
	EXPECT_CALL(ssd, read(0))
		.Times(200)
		.WillRepeatedly(Return(VALID_VALUE));
	EXPECT_CALL(ssd, read(99))
		.Times(200)
		.WillRepeatedly(Return(VALID_VALUE));

	EXPECT_CALL(ssd, write(0, _))
		.Times(200);
	EXPECT_CALL(ssd, write(99, _))
		.Times(200);

	string expected = "PASS";
	string actual = ts.writeReadAging(value);
	EXPECT_EQ(expected, actual);
}

TEST(ShellTest, invalidCommandTest) {
	string command = "readAfterWrite";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, invalidReadArgumentCount) {
	string command = "read 3 " + VALID_VALUE;
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}


TEST(ShellTest, invalidWriteArgumentCount) {
	string command = "write 50 ";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);

}

TEST(ShellTest, invalidWriteLba) {
	string command = "write 100 " + VALID_VALUE;
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, invalidHelpArgumentCount) {
	string command = "help help";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, invalidExitArgumentCount) {
	string command = "exi t";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, invalidFullReadArgumentCount) {
	string command = "fullread 3 " + VALID_VALUE;
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, invalidFullWriteArgumentCount) {
	string command = "full write " + VALID_VALUE;
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}


TEST(ShellTest, fullWriteFailWithInvalidValue) {
	string command = "fullwrite " + INVALID_VALUE;
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, TestCase1_WorkTest) {
	string command = "1_";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_TRUE(actual);
}

TEST(ShellTest, TestCase1_DoNotWorkTest) {
	string command = "1_dd";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, TestCase1_FullWorkTest) {
	string command = "1_FullWriteAndReadCompare";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_TRUE(actual);
}

TEST(ShellTest, TestCase1_FullDoNotWorkTest) {
	string command = "1_FullWriteAndReadComparedd";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, TestCase2_WorkTest) {
	string command = "2_";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_TRUE(actual);
}

TEST(ShellTest, TestCase2_DoNotWorkTest) {
	string command = "2_dasdfasdfd";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, TestCase2_FullWorkTest) {
	string command = "2_PartialLBAWrite";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_TRUE(actual);
}

TEST(ShellTest, TestCase2_FullDoNotWorkTest) {
	string command = "2_PartialLBAWrited";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, TestCase3_WorkTest) {
	string command = "3_";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_TRUE(actual);
}

TEST(ShellTest, TestCase3_DoNotWorkTest) {
	string command = "3_;";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

TEST(ShellTest, TestCase3_FullWorkTest) {
	string command = "3_WriteReadAging";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_TRUE(actual);
}

TEST(ShellTest, TestCase3_FullDoNotWorkTest) {
	string command = "3_WriteReadAginged";
	MockSSD ssd;
	TestShell ts{ &ssd };

	bool actual = ts.isValidCommandAndArgument(command);
	EXPECT_FALSE(actual);
}

int main(void) {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}