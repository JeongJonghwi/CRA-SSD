#pragma once
#include <cstdio>
#include <set>
#include <string>

using std::string;

class Logger {
public:
    virtual ~Logger();
    void Print(const string& logingFunctionStr, const string& loggingMessage);
    void CloseLogger();
    static Logger& getInstance();

private:
    Logger();
    bool OpenLastLogFile();
    bool IsOverLogFileSize() const;
    void FindFilesWithExtension(const string& folderPath, const string& extension);
    bool BackupLogFile();
    void WriteLog(const string& log) const;
    void RenameLogFile(const string& oldLogFileName, const string& newLogFileName) const;

    string GetCurTime(const char* format) const;
    std::tm GetCurrentTimeStruct() const;

    FILE* loggerFilePointer;
    string backupFileName;
    std::set<string> backupFileList;
};