#include "ssd.h"

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
    uint32_t lba, value;

    if (ssd.IsValidCheckAndCastType(argc, argv, &cmd, &lba, &value) == false) {
        ssd.WriteToOutputFileError();
        return 0;
    }

    switch (cmd) {
    case READ:
        ssd.Read(lba);
        break;
    case WRITE:
        ssd.Write(lba, value);
        break;
	case ERASE:
		if (value == 0) {
			break;
		}
		ssd.Erase(lba, value);
		break;
    }

    return 0;
}
#endif