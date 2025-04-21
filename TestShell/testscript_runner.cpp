#include "testscript_runner.h"
#include "test_script_interface.h"
#include <fstream>
#include <iostream>

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
            std::cout << "INVALID COMMAND\n";
            continue;
        }
        std::cout << cmd << "\t___\tRun...";
        if (testRun(cmd) != "PASS") {
            std::cout << "Fail!\n";
            break;
        }
        std::cout << "Pass\n";
    }
}

string TestScriptRunner::randomValue()
{
    unsigned int random_value = static_cast<unsigned int>(std::rand());
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << random_value;
    std::string hexStr = "0x" + ss.str();
    return hexStr;
}

string TestScriptRunner::testRun(string command)
{
    HMODULE hDll = LoadLibraryA("TestScript.dll");

    if (!hDll) {
        std::cerr << "Failed to load DLL!" << std::endl;
    }

    string scriptName = "CreateScript_" + command;
    CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(hDll, scriptName.c_str());
    if (!createScript) {
        return "FAIL";
    }

    ITestScript* script = createScript(ssd);
    string result = script->Run();
    delete script;
    FreeLibrary(hDll);

    return result;
}

bool TestScriptRunner::isTestScript(string command)
{
    HMODULE hDll = LoadLibraryA("TestScript.dll");

    if (!hDll) {
        std::cerr << "Failed to load DLL!" << std::endl;
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
        std::cout << "파일 열기 실패: " << filename << std::endl;
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
        lines.push_back(line);
    }

    fclose(file);
    return lines;
}