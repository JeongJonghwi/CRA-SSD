#include "ssd.h"
#include "command_buffer_manager.h"

#define TEST (1)
#if TEST
#include <iostream>
#include <cstring>
#endif

#ifndef _DEBUG
#include "gmock/gmock.h"

int main(void)
{
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}
#else
int main(int argc, char* argv[])
{
#if (TEST == 0)
    SSD ssd;
    CmdType cmd;
    CommandBufferManager buffermanager;
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
        // ssd.Write(lba, value);
        buffermanager.AddWrite(lba, value);
        break;
    }
    case ERASE:
    {
        if (value == 0) {
            break;
        }
        // ssd.Erase(lba, value);
        buffermanager.AddErase(lba, value);
        break;
    }
    }
#else
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
    CommandBufferManager buffermanager;
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
        // ssd.Write(lba, value);
        buffermanager.AddWrite(lba, value);
        break;
    }
    case ERASE:
    {
        if (value == 0) {
            break;
        }
        // ssd.Erase(lba, value);
        buffermanager.AddErase(lba, value);
        break;
    }
    }
#endif

    return 0;
}
#endif