#include "testscript_runner.h"

typedef ITestScript* (*CreateScriptFunc)(SSD*);

bool TestScriptRunner::isTestFile(string txt)
{
    const string extension = ".txt";
    if (txt.length() < extension.length()) {
        return false;
    }
    return txt.compare(txt.length() - extension.length(), extension.length(), extension) == 0;
}

void TestScriptRunner::txtFileTestRun(string filename)
{
    vector<string> cmds = readFileLines(filename);

    for (auto cmd : cmds) {
        if (!isTestScript(cmd)) {
            std::cout << "INVALID SCRIPT\n";
            logger.Print("TestScriptRunner.txtFileTestRun()", "CMD is Not in TestScript : " + cmd);
            return;
        }
        std::cout << cmd << "\t___\tRun...";
        if (testRun(cmd) != "PASS") {
            std::cout << "Fail!\n";
            return;
        }
        std::cout << "Pass\n";
    }
}

string TestScriptRunner::testRun(string command)
{
    HMODULE hDll = LoadLibraryA("TestScript.dll");

    if (!hDll) {
        logger.Print("TestScriptRunner.testRun()", "Failed to load DLL!");
    }

    string scriptName = "CreateScript_" + command;
    CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(hDll, scriptName.c_str());
    if (!createScript) {
        logger.Print("TestScriptRunner.testRun()", "CMD is Not in TestScript");
        return "FAIL";
    }

    ITestScript* script = createScript(ssd);

    logger.Print("TestScriptRunner.testRun()", "Test " + command + " Start!");
    string result = script->Run(&logger);
    logger.Print("TestScriptRunner.testRun()", "Test " + command + " End! result = " + result);
    delete script;
    FreeLibrary(hDll);

    return result;
}

bool TestScriptRunner::isTestScript(string command)
{
    HMODULE hDll = LoadLibraryA("TestScript.dll");

    if (!hDll) {
        logger.Print("TestScriptRunner.isTestScript()", "Failed to load DLL!");
    }

    string scriptName = "CreateScript_" + command;
    CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(hDll, scriptName.c_str());
    if (!createScript) {
        return false;
    }
    return true;
}

vector<string> TestScriptRunner::readFileLines(const string& filename)
{
    vector<string> lines;
    FILE* file = nullptr;

    if (fopen_s(&file, filename.c_str(), "r") != 0 || file == nullptr) {
        logger.Print("TestScriptRunner.readFileLines()", "Failed to Open File");
        return lines;
    }

    const size_t bufferSize = 1024;
    char buffer[bufferSize];

    while (fgets(buffer, bufferSize, file)) {
        string line(buffer);

        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
        }
        logger.Print("TestScriptRunner.readFileLines()", "Loaded Test Script : " + line);
        lines.push_back(line);
    }

    fclose(file);
    return lines;
}