#pragma once
#include <windows.h>
#include "interface.h"

class RealSSD : public SSD {

public:
	string read(uint32_t address) {
		std::string exeDir = getExecutablePath();
		std::string command = exeDir + "/SSD.exe R " + std::to_string(address);

		FILE* pipe = _popen(command.c_str(), "r");
		if (!pipe) {
			return "ERROR";
		}

		_pclose(pipe);
		
		FILE* file = nullptr;
		errno_t err = fopen_s(&file, "ssd_output.txt", "r");

		if (err != 0 || file == nullptr) {
			return "0x00000000";
		}

		char buffer[256];  // Max line length
		std::string firstLine = "";

		if (fgets(buffer, sizeof(buffer), file)) {
			firstLine = buffer;
		}

		return firstLine;
	}

	void write(uint32_t address, string value) {
		std::string exeDir = getExecutablePath();
		std::string command = exeDir + "/SSD.exe W " + std::to_string(address) + " " + value;
		FILE* pipe = _popen(command.c_str(), "r");
		if (pipe) {
			_pclose(pipe);
		}
	}

private:

	std::string getExecutablePath() {
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		std::string exePath(path);
		return exePath.substr(0, exePath.find_last_of("\\/"));
	}
};