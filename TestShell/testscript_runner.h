#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <random>

#include "ssd_interface.h"
#include "Logger.h"

using std::string;
using std::vector;

class TestScriptRunner {
public:
    TestScriptRunner(SSD* ssd)
        : ssd(ssd)
    {
    }
    bool isTestFile(string);
    void txtFileTestRun(string);
    string testRun(string);
    bool isTestScript(string);

private:
    Logger& logger = Logger::getInstance();
    vector<string> readFileLines(const string&);
    string randomValue();
    SSD *ssd;
};