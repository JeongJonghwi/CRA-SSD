#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <io.h>

#define LOGGING_LINE_SIZE 256
#define LOGGING_LAST_LOG_FILE_NAME "latest.log"
#define LOGGING_MAX_LOG_FILE_SIZE (10 * 1024)

Logger::Logger()
    : loggerFilePointer { nullptr }
    , backupFileName { "" }
    , backupFileList {}
{
    backupFileList.clear();
    OpenLastLogFile();
    FindFilesWithExtension(".\\", ".log");
    if (!backupFileList.empty()) {
        backupFileName = *backupFileList.begin();
        backupFileList.erase(backupFileList.begin());
    }
}

Logger::~Logger() {
    if (loggerFilePointer != nullptr) {
        fflush(loggerFilePointer);
        fclose(loggerFilePointer);
    }
}

void Logger::Print(const string& logingFunctionStr, const string& loggingMessage) {
    char messageChar[LOGGING_LINE_SIZE];
    string currentTimeStr = GetCurrentTime("%y.%m.%d %H:%M");

    snprintf(messageChar, LOGGING_LINE_SIZE, "%s %40s : %s", currentTimeStr.c_str(),
        logingFunctionStr.c_str(), loggingMessage.c_str());

    string messageStr { messageChar };

    WriteLog(messageStr);

    if (isOverLogFileSize()) {
        BackupLogFile();
    }
}

string Logger::GetCurrentTime(const char* format) const {
    // convert time_t to string
    std::tm localTime = GetCurrentTimeStruct();

    std::ostringstream oss;
    oss << std::put_time(&localTime, format); // 포맷 지정
    std::string timeString = "[" + oss.str() + "]";

    return timeString;
}

bool Logger::OpenLastLogFile() {
    if (_access_s(LOGGING_LAST_LOG_FILE_NAME, 0) != 0) {
        if (fopen_s(&loggerFilePointer, LOGGING_LAST_LOG_FILE_NAME, "w+") != 0) {
            return false;
        }
        return false;
    }

    if (fopen_s(&loggerFilePointer, LOGGING_LAST_LOG_FILE_NAME, "r+") != 0) {
        return false;
    }

    return true;
}

bool Logger::isOverLogFileSize() {
    fseek(loggerFilePointer, 0, SEEK_END);
    long fileSize = std::ftell(loggerFilePointer);

    return fileSize > static_cast<long>(LOGGING_MAX_LOG_FILE_SIZE);
}

void Logger::WriteLog(const string& log) {
    fprintf(loggerFilePointer, "%s\n", log.c_str());
    //fflush(loggerFilePointer);
}

void Logger::RenameLogFile(const string& oldLogFileName, const string& newLogFileName)
{
    rename(oldLogFileName.c_str(), newLogFileName.c_str());
}

bool Logger::BackupLogFile() {
    fclose(loggerFilePointer);
    std::tm localTime = GetCurrentTimeStruct();

    char currentTimeChar[LOGGING_LINE_SIZE];

    snprintf(currentTimeChar, LOGGING_LINE_SIZE, "until_%02d%02d%02d_%02dh_%02dm_%02ds.log", localTime.tm_year - 100,
        localTime.tm_mon, localTime.tm_mday, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
    string logFileName { currentTimeChar };

    if (backupFileName != "") {
        string zipBackupFileName = backupFileName.substr(0, backupFileName.length() - 4) + ".zip";
        RenameLogFile(backupFileName, zipBackupFileName);
    }
    backupFileName = logFileName;
    RenameLogFile(LOGGING_LAST_LOG_FILE_NAME, logFileName);
    if (fopen_s(&loggerFilePointer,LOGGING_LAST_LOG_FILE_NAME, "w") != 0) {
        return false;
    }

    return true;
}

std::tm Logger::GetCurrentTimeStruct() const
{
    // get current time
    auto now = std::chrono::system_clock::now();

    // convert current time to time_t
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // convert time_t to string
    std::tm localTime;
    localtime_s(&localTime, & currentTime);

    return localTime;
}

void Logger::FindFilesWithExtension(const string& directoryPath, const string& extension)
{
    _finddata_t fd;
    intptr_t handle;
    string path = directoryPath + "*" + extension;

    if ((handle = _findfirst(path.c_str(), &fd)) == -1L) {
        return;
    }

    do {
        string fileName = string(fd.name);

        if (fileName == LOGGING_LAST_LOG_FILE_NAME)
            continue;

        backupFileList.insert(fileName);
    } while (_findnext(handle, &fd) == 0);
    _findclose(handle);
}
