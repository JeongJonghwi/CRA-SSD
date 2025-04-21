#include "shell.h"

string TestShell::read(uint32_t address)
{
    if (!isValidAddress(address))
        return "ERROR";
    return ssd->read(address);
}

string TestShell::write(uint32_t address, string value)
{
    if (!isValidAddress(address))
        return "ERROR";
    if (!isValidValue(value))
        return "ERROR";

    ssd->write(address, value);
    return "Done";
}

string TestShell::help()
{
    string ret = "Team CodeCraft: ";
    ret += "강동현, 김태완, 유현승, 이수지, 정종휘\n";
    ret += "write: write {value} to {address}\n";
    ret += "read: read from {address}\n";
    ret += "fullwrite: write {value} to 0 ~ 99\n";
    ret += "fullread: read from all 0 ~ 99\n";
    ret += "help: print this message\n";
    ret += "exit: exit TestShell\n";
    return ret;
}

string TestShell::exit()
{
    return "Exit TestShell";
}

string TestShell::fullWrite(string value)
{
    if (!isValidValue(value))
        return "ERROR";
    for (int i = SSD_MINIMUM_ADDRESS; i < SSD_MAXIMUM_ADDRESS; i++) {
        ssd->write(i, value);
    }
    return "Done";
}

string TestShell::fullRead()
{
    string ret = "";
    for (int i = SSD_MINIMUM_ADDRESS; i < SSD_MAXIMUM_ADDRESS; i++) {
        ret += ssd->read(i);
        ret += "\n";
    }
    return ret;
}

string TestShell::erase(int address, int size)
{
    if (!isValidAddress(address))
        return "ERROR";
    if (size < 0)
        return "ERROR";
    ssd->erase(address, size);

    return "Done";
}

bool TestShell::isValidCommandAndArgument(string command)
{
    CMD cmd;
    return cmd.validCheck(command);
}

bool TestShell::isValidAddress(uint32_t address)
{
    if (address > 99 || address < 0)
        return false;
    return true;
}

bool TestShell::isValidValue(string value)
{
    if (value.length() != 10 || value[0] != '0' || value[1] != 'x')
        return false;

    for (int i = 2; i < 10; i++) {
        if ((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F') || (value[i] >= 'a' && value[i] <= 'f'))
            continue;
        return false;
    }
    return true;
}
