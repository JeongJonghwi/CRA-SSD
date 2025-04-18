#pragma once
#include <string>
#include <cstdio>
#include <set>

using std::string;

class Logger {
public:
    Logger();
    virtual ~Logger();
    void Print(const string& logingFunctionStr, const string& loggingMessage);

private:
    string GetCurrentTime(const char* format) const;
    bool OpenLastLogFile();
    bool isOverLogFileSize();
    void WriteLog(const string& log);
    void RenameLogFile(const string& oldLogFileName, const string& newLogFileName);
    bool BackupLogFile();
    std::tm* GetCurrentTimeStruct() const;
    void FindFilesWithExtension(const string& folderPath, const string& extension);

    FILE* loggerFilePointer;
    string backupFileName;
    std::set<string> backupFileList;
};