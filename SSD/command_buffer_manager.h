#pragma once

#include <string>
#include <list>
#include <stdint.h>
#include "ssd.h"

using std::string;
using std::list;

#define BUFFER_DIRECTORY_NAME "Buffer"
#define MAX_FILES (5)

struct Command {
    Command() : order{0}, lba{0}, value{0}, type{INVALID_CMD} {}
    bool operator<(const Command& other) const {
        return order > other.order;
    }

    uint32_t order;
    uint32_t lba;
    uint32_t value;
    CmdType type;
};

class CommandBufferManager {
public:
    CommandBufferManager();
    bool FastRead(uint32_t lba, uint32_t& readValue);
private:
    uint32_t valid_count;
    list<Command> commands;

    bool CheckDirectoryExists(const string& path);
    void CreateFolder();
    void ScanFiles();
    void CreateEmptyFile(const char* fileName);
};