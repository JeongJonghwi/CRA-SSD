#include "real_ssd.h"
#include "shell.cpp"
#include "testscript_runner.h"

#ifdef _DEBUG
#include "gmock/gmock.h"

int main(void)
{
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}
#else
string randomValue()
{
    unsigned int random_value = static_cast<unsigned int>(std::rand());
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << random_value;
    std::string hexStr = "0x" + ss.str();
    return hexStr;
}

int main(void)
{
    RealSSD ssd;
    TestShell test_shell(&ssd);
    TestScript script(&ssd);

    while (1) {
        string input;
        std::cout << "Shell > ";
        std::getline(std::cin, input);

        CMD cmd;
        TestScriptRunner runner(&ssd);

        if (cmd.validCheck(input)) {
            string command = cmd.getCommand();

            if (command == "read") {
                std::cout << test_shell.read(cmd.getAddress()) << std::endl;
            } else if (command == "write") {
                std::cout << test_shell.write(cmd.getAddress(), cmd.getValue()) << std::endl;
            } else if (command == "fullread") {
                std::cout << test_shell.fullRead() << std::endl;
            } else if (command == "fullwrite") {
                std::cout << test_shell.fullWrite(cmd.getValue()) << std::endl;
            } else if (command == "erase") {
                std::cout << test_shell.erase(cmd.getAddress(), cmd.getSize()) << std::endl;
            } else if (command == "erase_range") {
                std::cout << test_shell.erase(cmd.getAddress(), cmd.getSize()) << std::endl;
            } else if (command == "help") {
                std::cout << test_shell.help() << std::endl;
            } else if (command == "exit") {
                return 0;
            } else if (command == "1_" || command == "1_FullWriteAndReadCompare") {
                runner.testRun("1_");
            } else if (command == "2_" || command == "2_PartialLBAWrite") {
                runner.testRun("2_");
            } else if (command == "3_" || command == "3_WriteReadAging") {
                runner.testRun("3_");
            } else if (command == "4_" || command == "4_EraseAndWriteAging") {
                runner.testRun("4_");
            }
        } else if (runner.isTestFile(input)) {
            runner.txtFileTestRun(input);
        } else 
            std::cout << "INVALID COMMAND" << std::endl;
    }
}
#endif