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
    ret += "°­µ¿Çö, ±èÅÂ¿Ï, À¯Çö½Â, ÀÌ¼öÁö, Á¤Á¾ÈÖ\n";
    ret += "write [LBA] [Value] \t\t: write [Value] to [LBA]\n";
    ret += "read [LBA] \t\t\t: read from [LBA]\n";
    ret += "fullwrite [Value] \t\t: write [Value] to 0 ~ 99\n";
    ret += "fullread \t\t\t: read from all LBA (0 ~ 99)\n";
    ret += "erase [LBA] [SIZE] \t\t: erase Value starting at the [LBA] for the given [SIZE]\n";
    ret += "erase_range [startLBA] [endLBA] : erase Value ranging from the [startLBA] to the [endLBA]\n";
    ret += "flush \t\t\t\t: run and clear the command buffer\n";
    ret += "help \t\t\t\t: how to use the shell\n";
    ret += "exit \t\t\t\t: exit TestShell\n";
    logger.Print("TestShell.help()", ret);
    return ret;
}

string TestShell::exit()
{
    logger.Print("TestShell.exit()", "Shell Closed");
    return "Exit TestShell";
}

string TestShell::fullWrite(string value)
{
    if (!isValidValue(value))
        return "ERROR";
    logger.Print("TestShell.fullWrite()", "fullwrite start");
    for (int i = SSD_MINIMUM_ADDRESS; i < SSD_MAXIMUM_ADDRESS; i++) {
        ssd->write(i, value);
    }
    logger.Print("TestShell.fullWrite()", "fullwrite End");
    return "Done";
}

string TestShell::fullRead()
{
    string ret = "";
    logger.Print("TestShell.fullRead()", "fullRead start");
    for (int i = SSD_MINIMUM_ADDRESS; i < SSD_MAXIMUM_ADDRESS; i++) {
        ret += ssd->read(i);
        ret += "\n";
    }
    logger.Print("TestShell.fullRead()", "fullRead End");
    return ret;
}

string TestShell::erase(int address, int size)
{
    if (!isValidAddress(address))
        return "ERROR";
    if (size < 0)
        return "ERROR";
    logger.Print("TestShell.erase()", "erase start - address " + std::to_string(address) + " size " + std::to_string(size));
    ssd->erase(address, size);

    return "Done";
}

string TestShell::flush()
{
    ssd->flush();
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
