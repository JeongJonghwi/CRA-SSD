#pragma once
#include "interface.h"
#include <windows.h>

class RealSSD : public SSD {
public:
    RealSSD()
    {
        exeDir = getExecutablePath();
    }

    string read(uint32_t address)
    {
        std::string command = exeDir + "/SSD.exe R " + std::to_string(address);

        FILE* pipe = _popen(command.c_str(), "r");
        if (!pipe)
            return "ERROR";

        _pclose(pipe);

        FILE* file = nullptr;
        errno_t err = fopen_s(&file, "ssd_output.txt", "r");

        if (err != 0 || file == nullptr)
            return "0x00000000";

        char buffer[256];
        std::string firstLine = "";

        if (fgets(buffer, sizeof(buffer), file))
            firstLine = buffer;

        fclose(file);

        return firstLine;
    }

    void write(uint32_t address, string value)
    {
        std::string command = exeDir + "/SSD.exe W " + std::to_string(address) + " " + value;
        FILE* pipe = _popen(command.c_str(), "r");
        if (pipe)
            _pclose(pipe);
    }

private:
    std::string exeDir;

    std::string getExecutablePath()
    {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        std::string exePath(path);
        return exePath.substr(0, exePath.find_last_of("\\/"));
    }
};