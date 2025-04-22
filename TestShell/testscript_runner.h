#pragma once
#include "Logger.h"
#include "ssd_interface.h"
#include "test_script_interface.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

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
    SSD* ssd;
};