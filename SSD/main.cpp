#include "command_buffer_manager.h"
#include "ssd.h"

#define TEST (0)
#if (TEST == 0)
#ifdef _DEBUG
#include "gmock/gmock.h"

int main(void)
{
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}
#else
int main(int argc, char* argv[])
{
    SSD& ssd = SSD::GetInstance();
    CmdType cmd;
    CommandBufferManager& buffermanager = CommandBufferManager::GetInstance();
    uint32_t lba, value;

    if (ssd.ValidCheckAndCastType(argc, argv, &cmd, &lba, &value) == false) {
        ssd.WriteToOutputFileError();
        return 0;
    }

    switch (cmd) {
    case READ: {
        buffermanager.FastRead(lba);
        break;
    }
    case WRITE: {
        buffermanager.AddWrite(lba, value);
        break;
    }
    case ERASE: {
        if (value == 0) {
            break;
        }
        buffermanager.AddErase(lba, value);
        break;
    }
    case FLUSH: {
        buffermanager.Flush();
    }
    }

    return 0;
}
#endif
#else
#include <cstring>
#include <iostream>
int main(int argc, char* argv[])
{
    argc = 1;
    char input[5][20];
    while (true) {
        std::cout << "input: ";
        std::cin >> input[argc];

        if (strcmp(input[argc], "exit") == 0)
            break;

        argv[argc] = input[argc];
        argc++;
    }

    SSD& ssd = SSD::GetInstance();
    CmdType cmd;
    CommandBufferManager& buffermanager = CommandBufferManager::GetInstance();
    uint32_t lba, value;

    if (ssd.ValidCheckAndCastType(argc, argv, &cmd, &lba, &value) == false) {
        ssd.WriteToOutputFileError();
        return 0;
    }

    switch (cmd) {
    case READ: {
        buffermanager.FastRead(lba);
        break;
    }
    case WRITE: {
        buffermanager.AddWrite(lba, value);
        break;
    }
    case ERASE: {
        if (value == 0) {
            break;
        }
        buffermanager.AddErase(lba, value);
        break;
    }
    case FLUSH: {
        buffermanager.Flush();
    }
    }

    return 0;
}
#endif