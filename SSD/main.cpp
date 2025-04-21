#include "ssd.h"
#include "command_buffer_manager.h"

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
    SSD ssd;
    CmdType cmd;
    CommandBufferManager buffermanager { &ssd };
    uint32_t lba, value;

    if (ssd.ValidCheckAndCastType(argc, argv, &cmd, &lba, &value) == false) {
        ssd.WriteToOutputFileError();
        return 0;
    }

    switch (cmd) {
    case READ:
    {
        uint32_t readValue;
        if (buffermanager.FastRead(lba, readValue)) {
            if (ssd.WriteToOutputFile(readValue) == false) {
                return 0;
            }
        }
        else {
            ssd.Read(lba);
        }
        break;
    }
    case WRITE:
    {
        buffermanager.AddWrite(lba, value);
        break;
    }
    case ERASE:
    {
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
#include <iostream>
#include <cstring>
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
    SSD ssd;
    CmdType cmd;
    CommandBufferManager buffermanager { &ssd };
    uint32_t lba, value;

    if (ssd.ValidCheckAndCastType(argc, argv, &cmd, &lba, &value) == false) {
        ssd.WriteToOutputFileError();
        return 0;
    }

    switch (cmd) {
    case READ:
    {
        uint32_t readValue;
        if (buffermanager.FastRead(lba, readValue)) {
            if (ssd.WriteToOutputFile(readValue) == false) {
                return 0;
            }
        }
        else {
            ssd.Read(lba);
        }
        break;
    }
    case WRITE:
    {
        buffermanager.AddWrite(lba, value);
        break;
    }
    case ERASE:
    {
        if (value == 0) {
            break;
        }
        buffermanager.AddErase(lba, value);
        break;
    }
    }

    return 0;
}
#endif