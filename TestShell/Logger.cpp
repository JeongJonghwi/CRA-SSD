#include "Logger.h"
#include <chrono>
#include <ctime>
#include <io.h>
#include <iomanip>
#include <iostream>
#include <sstream>

#define LOGGING_LINE_SIZE 512
#define LOGGING_LAST_LOG_FILE_NAME "latest.log"
#define LOGGING_MAX_LOG_FILE_SIZE (10 * 1024)
#define LOGGING_FILE_EXTENSION_SIZE 4

Logger::Logger()
    : loggerFilePointer { nullptr }
    , backupFileName { "" }
    , backupFileList {}
{
    backupFileList.clear();

    OpenLastLogFile();
    FindFilesWithExtension(".\\", ".log");

    if (backupFileList.empty() == false) {
        backupFileName = *backupFileList.begin();
        backupFileList.erase(backupFileList.begin());
    }
}

Logger::~Logger()
{
    CloseLogger();
}

void Logger::CloseLogger()
{
    backupFileList.clear();
    if (loggerFilePointer != nullptr) {
        fflush(loggerFilePointer);
        fclose(loggerFilePointer);
        loggerFilePointer = nullptr;
    }
}

// Singleton
Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::Print(const string& logingFunctionStr, const string& loggingMessage)
{
    char messageChar[LOGGING_LINE_SIZE];
    string currentTimeStr = GetCurTime("%y.%m.%d %H:%M");

    snprintf(messageChar, LOGGING_LINE_SIZE, "%s %-40s : %s", currentTimeStr.c_str(),
        logingFunctionStr.c_str(), loggingMessage.c_str());

    string messageStr { messageChar };

    WriteLog(messageStr);

    if (IsOverLogFileSize()) {
        BackupLogFile();
    }
}

bool Logger::OpenLastLogFile()
{
    if (_access_s(LOGGING_LAST_LOG_FILE_NAME, 0) != 0) { // if not exist lastlog file
        if (fopen_s(&loggerFilePointer, LOGGING_LAST_LOG_FILE_NAME, "w+") != 0) {
            return false;
        }
        return true;
    }

    if (fopen_s(&loggerFilePointer, LOGGING_LAST_LOG_FILE_NAME, "a+") != 0) {
        return false;
    }

    return true;
}

void Logger::FindFilesWithExtension(const string& directoryPath, const string& extension)
{
    _finddata_t fd;
    intptr_t handle;
    string path = directoryPath + "*" + extension;

    // find the *.log file in directory path
    if ((handle = _findfirst(path.c_str(), &fd)) == -1L) {
        return;
    }

    // get the backup file
    do {
        string fileName = string(fd.name);

        if (fileName == LOGGING_LAST_LOG_FILE_NAME)
            continue;

        backupFileList.insert(fileName);
    } while (_findnext(handle, &fd) == 0);

    _findclose(handle);
}

bool Logger::IsOverLogFileSize() const
{
    fseek(loggerFilePointer, 0, SEEK_END);
    long fileSize = ftell(loggerFilePointer);

    return fileSize > static_cast<long>(LOGGING_MAX_LOG_FILE_SIZE);
}

bool Logger::BackupLogFile()
{
    fclose(loggerFilePointer);

    // get log file name
    std::tm localTime = GetCurrentTimeStruct();
    char currentTimeChar[LOGGING_LINE_SIZE];

    snprintf(currentTimeChar, LOGGING_LINE_SIZE, "until_%02d%02d%02d_%02dh_%02dm_%02ds.log", localTime.tm_year - 100,
        localTime.tm_mon, localTime.tm_mday, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
    string logFileName { currentTimeChar };

    // convert *.log file to *.zip file
    if (backupFileName != "") {
        string zipBackupFileName = backupFileName.substr(0, backupFileName.length() - LOGGING_FILE_EXTENSION_SIZE) + ".zip";
        RenameLogFile(backupFileName, zipBackupFileName);
    }
    backupFileName = logFileName;

    // convert lastlog file to *.log file
    RenameLogFile(LOGGING_LAST_LOG_FILE_NAME, logFileName);

    // open lastlog file
    if (fopen_s(&loggerFilePointer, LOGGING_LAST_LOG_FILE_NAME, "w") != 0) {
        return false;
    }

    return true;
}

void Logger::WriteLog(const string& log) const
{
    fprintf(loggerFilePointer, "%s\n", log.c_str());
}

void Logger::RenameLogFile(const string& oldLogFileName, const string& newLogFileName) const
{
    int ret = rename(oldLogFileName.c_str(), newLogFileName.c_str());
    if (ret)
        exit(EXIT_FAILURE);
}

string Logger::GetCurTime(const char* format) const
{
    std::tm localTime = GetCurrentTimeStruct();

    // convert time_t to string
    std::ostringstream oss;
    oss << std::put_time(&localTime, format);
    std::string timeString = "[" + oss.str() + "]";

    return timeString;
}

std::tm Logger::GetCurrentTimeStruct() const
{
    // get current time
    auto now = std::chrono::system_clock::now();

    // convert current time to time_t
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // convert time_t to string
    std::tm localTime;
    localtime_s(&localTime, &currentTime);

    return localTime;
}
