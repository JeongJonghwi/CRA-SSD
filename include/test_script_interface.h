#pragma once
#include <iostream>

class ITestScript {
public:
    virtual ~ITestScript() { }
    virtual std::string Run() = 0;
};