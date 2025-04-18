#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <random>

#include "testscript.cpp"

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
    bool testRun(string);

private:
    vector<string> readFileLines(const string&);
    bool isTestScript(string);
    string randomValue();
    SSD *ssd;
};