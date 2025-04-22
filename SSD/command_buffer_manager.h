#pragma once

#include "ssd.h"
#include <list>
#include <stdint.h>
#include <string>

using std::list;
using std::string;
using std::to_string;

#define BUFFER_DIRECTORY_NAME "Buffer"
#define MAX_FILES (5)

struct Command {
    Command()
        : order { 0 }
        , lba { 0 }
        , value { 0 }
        , type { INVALID_CMD }
    {
    }
    Command(uint32_t order, uint32_t lba, uint32_t value, CmdType type)
        : order { order }
        , lba { lba }
        , value { value }
        , type { type }
    {
    }
    bool operator<(const Command& other) const
    {
        return order > other.order;
    }
    uint32_t GetEnd() const
    {
        return lba + value - 1;
    }

    uint32_t order;
    uint32_t lba;
    uint32_t value;
    CmdType type;
};

class CommandBufferManager {
public:
    CommandBufferManager(SSD* ssd);
    void FastRead(uint32_t lba);
    void AddWrite(uint32_t lba, uint32_t value);
    void AddErase(uint32_t lba, uint32_t value);
    bool Flush();

private:
    SSD* ssd;
    uint32_t valid_count;
    list<Command> commands;

    bool CheckDirectoryExists(const string& path);
    void CreateFolder();
    void ScanFiles();
    void CreateEmptyFile(const char* fileName);

    list<Command>::iterator Delete(list<Command>::iterator iter);
    string GetFileName(Command& command);
    string GetFileName(uint32_t order, CmdType type, uint32_t lba, uint32_t value);
    void Rename(list<Command>::iterator iter, int32_t order, CmdType type, uint32_t lba, uint32_t value);
    void Invalidate(Command& command);
    void AddCommand(CmdType type, uint32_t lba, uint32_t value);
    void MergeErase();
};