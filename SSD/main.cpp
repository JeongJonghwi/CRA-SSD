#include "ssd.h"

#if _DEBUG
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
    unsigned int nLba, nValue;

    if (ssd.isValidCheckAndCastType(argc, argv, &cmd, &nLba, &nValue) == false) {
        ssd.WriteToOutputFileError();
        return 0;
    }

    switch (cmd) {
    case READ:
        ssd.Read(nLba);
        break;
    case WRITE:
        ssd.Write(nLba, nValue);
        break;
    }

    return 0;
}
#endif