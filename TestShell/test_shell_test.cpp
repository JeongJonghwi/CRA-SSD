#include "real_ssd.h"
#include "shell.h"
#include "ssd_interface.h"
#include "test_script_interface.h"
#include "gmock/gmock.h"

using namespace testing;
using std::string;

const int VALID_ADDRESS = 3;
const int INVALID_ADDRESS = 100;
const string VALID_VALUE = "0x12345678";
const string INVALID_VALUE = "0xTTTTFFFF";

class MockSSD : public SSD {
public:
    MOCK_METHOD(string, read, (uint32_t address), (override));
    MOCK_METHOD(void, write, (uint32_t address, string value), (override));
    MOCK_METHOD(void, erase, (uint32_t address, uint32_t size), (override));
};

class TestFixture : public Test {
public:
    MockSSD ssd;
    TestShell ts { &ssd };
};

class SSDFixture : public Test {
public:
    RealSSD ssd;
    TestShell ts { &ssd };
};

class ShellTestFixture : public TestFixture { };

typedef ITestScript* (*CreateScriptFunc)(SSD*);

class ShellTestScriptFixture : public TestFixture {

public:
    string launchDLLfunction(string scriptName)
    {
        HMODULE hDll = LoadLibraryA("TestScript.dll");

        if (!hDll) {
            return "Failed to load DLL!";
        }

        CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(hDll, ("CreateScript_" + scriptName).c_str());
        if (!createScript) {
            return "Failed to get CreateScript function!";
        }

        ITestScript* script = createScript(&ssd);
        string actual = script->Run();
        delete script;
        FreeLibrary(hDll);
        return actual;
    }
};

class InvalidCMDTestFixture : public TestFixture {
public:
    void validNotOkay(string command)
    {
        EXPECT_FALSE(ts.isValidCommandAndArgument(command));
    }
    void validOkay(string command)
    {
        EXPECT_TRUE(ts.isValidCommandAndArgument(command));
    }
};

TEST_F(ShellTestFixture, readSuccess)
{
    EXPECT_CALL(ssd, read(_))
        .Times(1)
        .WillRepeatedly(Return("0x10000000"));

    string expected = "0x10000000";
    string actual = ts.read(VALID_ADDRESS);

    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestFixture, readFailWithInvalidLBA)
{
    EXPECT_EQ("ERROR", ts.read(INVALID_ADDRESS));
}

TEST_F(ShellTestFixture, writeSuccess)
{
    EXPECT_CALL(ssd, write(_, _))
        .Times(1);

    string expected = "Done";
    string actual = ts.write(VALID_ADDRESS, VALID_VALUE);
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestFixture, writeFailWithInvalidLBA)
{
    EXPECT_CALL(ssd, write(_, _))
        .Times(0);

    string expected = "ERROR";
    string actual = ts.write(INVALID_ADDRESS, VALID_VALUE);
    EXPECT_EQ("ERROR", ts.write(INVALID_ADDRESS, VALID_VALUE));
}

TEST_F(ShellTestFixture, writeFailWithInvalidValue)
{
    EXPECT_CALL(ssd, write(_, _))
        .Times(0);

    string expected = "ERROR";
    string actual = ts.write(VALID_ADDRESS, INVALID_VALUE);
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestFixture, eraseSuccess)
{
    EXPECT_CALL(ssd, erase(VALID_ADDRESS, 1))
        .Times(1);

    string expected = "Done";
    string actual = ts.erase(VALID_ADDRESS, 1);
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestFixture, eraseFailWithInvalidSize)
{
    EXPECT_CALL(ssd, erase(VALID_ADDRESS, -2))
        .Times(0);

    string expected = "ERROR";
    string actual = ts.erase(VALID_ADDRESS, -2);
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestFixture, eraseFailWithInvalidLBA)
{
    EXPECT_CALL(ssd, erase(INVALID_ADDRESS, 1))
        .Times(0);

    string expected = "ERROR";
    string actual = ts.erase(INVALID_ADDRESS, 1);
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestFixture, helpTest)
{
    string expected = "Team CodeCraft: ";
    expected += "°­µ¿Çö, ±èÅÂ¿Ï, À¯Çö½Â, ÀÌ¼öÁö, Á¤Á¾ÈÖ\n";
    expected += "write: write {value} to {address}\n";
    expected += "read: read from {address}\n";
    expected += "fullwrite: write {value} to 0 ~ 99\n";
    expected += "fullread: read from all 0 ~ 99\n";
    expected += "help: print this message\n";
    expected += "exit: exit TestShell\n";

    EXPECT_EQ(expected, ts.help());
}

TEST_F(ShellTestFixture, exitTest)
{
    EXPECT_EQ("Exit TestShell", ts.exit());
}

TEST_F(ShellTestFixture, fullWriteTest)
{
    EXPECT_CALL(ssd, write(_, _))
        .Times(100);

    ts.fullWrite(VALID_VALUE);
}

TEST_F(ShellTestFixture, fullReadTest)
{
    EXPECT_CALL(ssd, read(_))
        .Times(100)
        .WillRepeatedly(Return("0x00000000"));

    ts.fullRead();
}

TEST_F(ShellTestScriptFixture, fullWriteAndReadTest)
{
    for (int groupStart = 0; groupStart < 100; groupStart += 5) {

        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << groupStart;
        string value = "0x123456" + ss.str();
        std::transform(value.begin() + 2, value.end(), value.begin() + 2, ::toupper);

        for (int subIndex = 0; subIndex < 5; subIndex++) {
            uint32_t lba = groupStart + subIndex;
            EXPECT_CALL(ssd, write(lba, _))
                .Times(1);
            EXPECT_CALL(ssd, read(lba))
                .Times(1)
                .WillRepeatedly(Return(value));
        }
    }

    string expected = "PASS";
    string actual = launchDLLfunction("1_FullWriteAndReadCompare");
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestScriptFixture, partialLBAWriteTest)
{
    EXPECT_CALL(ssd, write(_, _))
        .Times(150);

    EXPECT_CALL(ssd, read(_))
        .Times(150)
        .WillRepeatedly(Return(VALID_VALUE));

    string expected = "PASS";
    string actual = launchDLLfunction("2_PartialLBAWrite");
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestScriptFixture, writeReadAging)
{
    EXPECT_CALL(ssd, read(0))
        .Times(200)
        .WillRepeatedly(Return(VALID_VALUE));

    EXPECT_CALL(ssd, read(99))
        .Times(200)
        .WillRepeatedly(Return(VALID_VALUE));

    EXPECT_CALL(ssd, write(0, _)).Times(200);

    EXPECT_CALL(ssd, write(99, _)).Times(200);

    string expected = "PASS";
    string actual = launchDLLfunction("3_WriteReadAging");
    EXPECT_EQ(expected, actual);
}

TEST_F(ShellTestScriptFixture, eraseAndWriteAging)
{
    for (int addr = 0; addr < 99; addr++) {
        EXPECT_CALL(ssd, read(addr)).WillRepeatedly(Return("0x00000000"));
    }

    EXPECT_CALL(ssd, erase(0, 3)).Times(1);

    for (int startAddr = 2; startAddr <= 96; startAddr += 2) {
        EXPECT_CALL(ssd, write(startAddr, _)).Times(60);
        EXPECT_CALL(ssd, erase(startAddr, 3)).Times(30);
    }

    string expected = "PASS";
    string actual = launchDLLfunction("4_EraseAndWriteAging");
    EXPECT_EQ(expected, actual);
}

TEST_F(InvalidCMDTestFixture, invalidCommandTest)
{
    validNotOkay("readAfterWrite");
}

TEST_F(InvalidCMDTestFixture, invalidReadArgumentCount)
{
    validNotOkay("read 3 " + VALID_VALUE);
}

TEST_F(InvalidCMDTestFixture, invalidWriteArgumentCount)
{
    validNotOkay("write 50 ");
}

TEST_F(InvalidCMDTestFixture, invalidWriteLba)
{
    validNotOkay("write 100 " + VALID_VALUE);
}

TEST_F(InvalidCMDTestFixture, invalidHelpArgumentCount)
{
    validNotOkay("help help");
}

TEST_F(InvalidCMDTestFixture, invalidExitArgumentCount)
{
    validNotOkay("exi t");
}

TEST_F(InvalidCMDTestFixture, invalidFullReadArgumentCount)
{
    validNotOkay("fullread 3 " + VALID_VALUE);
}

TEST_F(InvalidCMDTestFixture, invalidFullWriteArgumentCount)
{
    validNotOkay("full write " + VALID_VALUE);
}

TEST_F(InvalidCMDTestFixture, fullWriteFailWithInvalidValue)
{
    validNotOkay("fullwrite " + INVALID_VALUE);
}

TEST_F(SSDFixture, writeAndReadWithRealSSD)
{
    string expected = "Done";
    string actual = ts.write(VALID_ADDRESS, VALID_VALUE);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(ts.read(VALID_ADDRESS), VALID_VALUE);
}

TEST_F(SSDFixture, writeFailAndReadWithRealSSD)
{
    ts.write(VALID_ADDRESS, VALID_VALUE);

    string expected = "ERROR";
    string actual = ts.write(VALID_ADDRESS, INVALID_VALUE);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(ts.read(VALID_ADDRESS), VALID_VALUE);
}

TEST_F(SSDFixture, eraseAndReadWithRealSSD)
{
    string expected = "Done";
    string actual = ts.erase(VALID_ADDRESS, 1);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(ts.read(VALID_ADDRESS), "0x00000000");
}

TEST_F(SSDFixture, eraseFailAndReadWithRealSSD)
{
    ts.write(VALID_ADDRESS, VALID_VALUE);

    string expected = "ERROR";
    string actual = ts.erase(VALID_ADDRESS, -2);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(ts.read(VALID_ADDRESS), VALID_VALUE);
}
