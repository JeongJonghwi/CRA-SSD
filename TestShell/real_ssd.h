#pragma once
#include "interface.h"
#include <windows.h>

using std::to_string;

class RealSSD : public SSD {
public:
    RealSSD()
    {
        exeFile = GetExecutablePath() + "/SSD.exe ";
    }

    string read(uint32_t address)
    {
        RunCommand("R " + to_string(address));
        return ReadFromOutputFile();
    }

    void write(uint32_t address, string value)
    {
        RunCommand("W " + to_string(address) + " " + value);
    }

    void erase(uint32_t address, uint32_t size)
    {
        while (size > 10) {
            RunCommand("E " + to_string(address) + " 10");
            size -= 10;
            address += 10;
        }
        RunCommand("E " + to_string(address) + " " + to_string(size));
    }

private:
    string exeFile;

    string GetExecutablePath()
    {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        string exePath(path);
        return exePath.substr(0, exePath.find_last_of("\\/"));
    }

    void RunCommand(string command)
    {
        string fullCommand = exeFile + command;
        system((exeFile + command).c_str());
    }

    string ReadFromOutputFile()
    {
        FILE* file = nullptr;
        errno_t err = fopen_s(&file, "ssd_output.txt", "r");

        if (err != 0 || file == nullptr)
            return "0x00000000";

        char buffer[256];
        string firstLine = "";

        if (fgets(buffer, sizeof(buffer), file))
            firstLine = buffer;

        fclose(file);

        return firstLine;
    }
};