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

        int result = system(command.c_str());
        if (result)
            return "ERROR";

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
        system(command.c_str());
    }

    void erase(uint32_t address, uint32_t size)
    {
        while (size > 10) {
            std::string command = exeDir + "/SSD.exe E " + std::to_string(address) + " 10";
            FILE* pipe = _popen(command.c_str(), "r");
            if (pipe)
                _pclose(pipe);

            size -= 10;
            address += 10;
        }
        std::string command = exeDir + "/SSD.exe E " + std::to_string(address) + " " + std::to_string(size);
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