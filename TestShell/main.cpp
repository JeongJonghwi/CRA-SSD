#include "real_ssd.h"
#include "test_shell.cpp"
#include <random>

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

    while (1) {
        string input;
        std::cout << "Shell > ";
        std::getline(std::cin, input);

        CMD cmd;
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
            } else if (command == "help") {
                std::cout << test_shell.help() << std::endl;
            } else if (command == "exit") {
                return 0;
            } else if (command == "1_" || command == "1_FullWriteAndReadCompare") {
                std::cout << test_shell.fullWriteAndReadCompare() << std::endl;
            } else if (command == "2_" || command == "2_PartialLBAWrite") {
                std::cout << test_shell.partialLBAWrite("0xFFFFFFFF") << std::endl;
            } else if (command == "3_" || command == "3_WriteReadAging") {
                std::cout << test_shell.writeReadAging(randomValue()) << std::endl;
            }
        } else
            std::cout << "INVALID COMMAND" << std::endl;
    }
}
#endif