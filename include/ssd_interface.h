#pragma once
#include <string>

using std::string;

#define interface struct

interface SSD
{
public:
    virtual string read(uint32_t address) = 0;
    virtual void write(uint32_t address, string value) = 0;
    virtual void erase(uint32_t address, uint32_t size) = 0;
};