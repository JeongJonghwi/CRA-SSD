#include "command_buffer_manager.h"
#include <windows.h>
#include <cstdlib>
#include <io.h>

CommandBufferManager::CommandBufferManager()
{
    if (CheckDirectoryExists(BUFFER_DIRECTORY_NAME)) {
        CreateFolder();
        valid_count = 0;
    } else {
        ScanFiles();
        valid_count = static_cast<uint32_t>(commands.size());
    }
}

bool CommandBufferManager::FastRead(uint32_t lba, uint32_t& readValue)
{
    for (const auto& command : commands)
    {
        if (command.type == WRITE) {
            if (command.lba == lba) {
                readValue = command.value;
                return true;
            }
        }
        else if (command.type == ERASE) {
            if (lba >= command.lba && lba < command.lba + command.value) {
                readValue = 0;
                return true;
            }
        }
    }
    
    return false;
}

bool CommandBufferManager::CheckDirectoryExists(const string& path)
{
    std::wstring directoryPath;
    directoryPath.assign(path.begin(), path.end());
    DWORD attributes = GetFileAttributes(directoryPath.c_str());
    return (attributes == INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}

void CommandBufferManager::CreateFolder()
{
    string path { BUFFER_DIRECTORY_NAME };
    std::wstring directoryPath;
    directoryPath.assign(path.begin(), path.end());
    CreateDirectory(directoryPath.c_str(), NULL);

    for (int i = 1; i <= MAX_FILES; i++) {
        string fileName = path;
        fileName += "\\" + std::to_string(i) + "_empty";
        CreateEmptyFile(fileName.c_str());
    }
}

void CommandBufferManager::CreateEmptyFile(const char* fileName)
{
    FILE* fp = nullptr;

    if ((fopen_s(&fp, fileName, "w+")) != 0) {
        return;
    }

    fclose(fp);
}

void CommandBufferManager::ScanFiles()
{
    string path = string(BUFFER_DIRECTORY_NAME) + "\\*";
    _finddata_t fd;
    intptr_t handle;

    if ((handle = _findfirst(path.c_str(), &fd)) == -1L) {
        exit(0);
    }

    do {
        string filename = string(fd.name);
        if (filename[0] != '.' && filename.find("empty") == string::npos) {
            Command command;
            size_t start;
            size_t end;

            start = 0;
            end = filename.find('_');
            command.order = stoi(filename.substr(start, end-start));

            start = end + 1;
            end = filename.find('_', start);
            command.type = static_cast<CmdType>(SSD::GetCmdType(filename.substr(start, end-start)));

            start = end + 1;
            end = filename.find('_', start);
            command.lba = stoi(filename.substr(start, end-start));

            start = end + 1;
            end = filename.find('_', start);
            command.value = stoi(filename.substr(start, end-start));
        
            commands.push_back(command);
        }
    } while (_findnext(handle, &fd) == 0);
    _findclose(handle);

    commands.sort();
}