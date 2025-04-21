#include "command_buffer_manager.h"
#include <windows.h>
#include <cstdlib>
#include <io.h>
#include <algorithm>

using std::min;
using std::max;

CommandBufferManager::CommandBufferManager(SSD* ssd)
{
    this->ssd = ssd;
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
            if (lba >= command.lba && lba <= command.GetEnd()) {
                readValue = 0;
                return true;
            }
        }
    }
    
    return false;
}

void CommandBufferManager::AddWrite(uint32_t lba, uint32_t value)
{
    if (commands.size() >= MAX_FILES) {
        Flush();
    }

    for (list<Command>::iterator iter = commands.begin(); iter != commands.end();)
    {
        if (iter->lba == lba) {
            iter = Delete(iter);
            continue;
        }
        iter++;
    }

    AddCommand(WRITE, lba, value);
    
    MergeErase();
}

void CommandBufferManager::AddErase(uint32_t lba, uint32_t value)
{
    if (commands.size() >= MAX_FILES) {
        Flush();
    }

    for (list<Command>::iterator iter = commands.begin(); iter != commands.end();) {
        if (iter->type == WRITE) {
            if ((iter->lba >= lba) && (iter->lba < lba + value)) {
                iter = Delete(iter);
                continue;
            }
        } else { // if (iter->type == ERASE)
            if ((iter->lba >= lba) && (iter->GetEnd() <= lba + value - 1)) {
                iter = Delete(iter);
                continue;
            }
        }
        iter++;
    }

    AddCommand(ERASE, lba, value);

    MergeErase();
}

bool CommandBufferManager::Flush() {
    for (list<Command>::reverse_iterator iter = commands.rbegin(); iter != commands.rend(); iter++) {
        switch (iter->type) {
        case WRITE:
            ssd->Write(iter->lba, iter->value);
            break;
        case ERASE:
            ssd->Erase(iter->lba, iter->value);
            break;
        default:
            break;
        }
        Invalidate(*iter);
    }
    commands.clear();

    return true;
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
            command.order = static_cast<uint32_t>(stoul(filename.substr(start, end-start)));

            start = end + 1;
            end = filename.find('_', start);
            command.type = static_cast<CmdType>(SSD::GetCmdType(filename.substr(start, end-start)));

            start = end + 1;
            end = filename.find('_', start);
            command.lba = static_cast<uint32_t>(stoul(filename.substr(start, end-start)));

            start = end + 1;
            end = filename.find('_', start);
            command.value = static_cast<uint32_t>(stoul(filename.substr(start, end-start)));    // decimal
        
            commands.push_back(command);
        }
    } while (_findnext(handle, &fd) == 0);
    _findclose(handle);

    commands.sort();
}

list<Command>::iterator CommandBufferManager::Delete(list<Command>::iterator iter)
{
    Invalidate(*iter);
    iter = commands.erase(iter);

    if (iter == commands.begin() || commands.size() == 0) {
        return iter;
    }

    list<Command>::iterator it = iter;
    for (it--; it != commands.begin(); it--) {
        Rename(it, it->order-1, it->type, it->lba, it->value);
        it->order--;
    }

    return iter;
}

string CommandBufferManager::GetFileName(Command &command)
{
    string combined = string(BUFFER_DIRECTORY_NAME) + "\\";
    combined += to_string(command.order);
    combined += "_" + cmd_symbol[command.type];
    combined += "_" + to_string(command.lba);
    combined += "_" + to_string(command.value); // decimal
    return combined;
}

string CommandBufferManager::GetFileName(uint32_t order, CmdType type, uint32_t lba, uint32_t value)
{
    string combined = string(BUFFER_DIRECTORY_NAME) + "\\";
    combined += to_string(order);
    combined += "_" + cmd_symbol[type];
    combined += "_" + to_string(lba);
    combined += "_" + to_string(value);         // decimal
    return combined;
}

void CommandBufferManager::Rename(list<Command>::iterator iter, int32_t order, CmdType type, uint32_t lba, uint32_t value)
{
    string old = GetFileName(*iter);
    string changed = GetFileName(order, type, lba, value);
    rename(old.c_str(), changed.c_str());
}

void CommandBufferManager::Invalidate(Command& command)
{
    string old = GetFileName(command);
    string changed = string(BUFFER_DIRECTORY_NAME) + "\\" + to_string(valid_count--) + "_empty";
    rename(old.c_str(), changed.c_str());
}

void CommandBufferManager::AddCommand(CmdType type, uint32_t lba, uint32_t value)
{
    string old = string(BUFFER_DIRECTORY_NAME) + "\\" + to_string(++valid_count) + "_empty";
    string changed = GetFileName(valid_count, type, lba, value);
    rename(old.c_str(), changed.c_str());
    commands.emplace_front(valid_count, lba, value, type);
}

void CommandBufferManager::MergeErase()
{
    for (list<Command>::iterator iter = commands.begin(); iter != commands.end();)
    {
        if (iter->type == ERASE) {
            list<Command>::iterator next = iter;
            next++;
            if (next == commands.end() || next->type != ERASE) {
                break;
            }

            uint32_t start1 = iter->lba;
            uint32_t start2 = next->lba;
            uint32_t end1 = iter->GetEnd();
            uint32_t end2 = next->GetEnd();

            if (end1 >= start2 - 1 && end2 >= start1 - 1) {
                uint32_t new_start = min(start1, start2);
                uint32_t new_end = max(end1, end2);
                uint32_t new_size = new_end - new_start + 1;
                
                if (new_size > MAX_COUNT) {
                    iter = next;
                    continue;
                }
                
                Delete(iter);
                Rename(next, next->order, next->type, new_start, new_size);
                next->lba = new_start;
                next->value = new_size;
                iter = next;
                continue;
            }
        }

        iter++;
    }
}