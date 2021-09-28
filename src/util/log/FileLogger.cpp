#include "FileLogger.h"
using namespace std;
FileLogger::FileLogger(std::string logName) {
    fp = fopen(logName.c_str(), "w+");
}

FileLogger::~FileLogger() { fclose(fp); }

void FileLogger::verbose(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    fprintf(fp, "[VERBOSE] %s %s:%d %s\n", getCurrentTime().c_str(),
            filename.c_str(), line, s.c_str());
    fflush(fp);
}

void FileLogger::info(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    fprintf(fp, "[INFO]    %s %s:%d %s\n", getCurrentTime().c_str(),
            filename.c_str(), line, s.c_str());
    fflush(fp);
}
void FileLogger::warning(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    fprintf(fp, "[WARNING] %s %s:%d %s\n", getCurrentTime().c_str(),
            filename.c_str(), line, s.c_str());
    fflush(fp);
}
void FileLogger::error(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    fprintf(fp, "[ERROR]   %s %s:%d %s\n", getCurrentTime().c_str(),
            filename.c_str(), line, s.c_str());
    fflush(fp);
}

void FileLogger::verbosef(std::string filename, int line, const char* format,
                          ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[VERBOSE] %s %s:%d ", getCurrentTime().c_str(),
            filename.c_str(), line);
    vfprintf(fp, format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
    fflush(fp);
}
void FileLogger::infof(std::string filename, int line, const char* format,
                       ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[INFO]    %s %s:%d ", getCurrentTime().c_str(),
            filename.c_str(), line);
    vfprintf(fp, format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
    fflush(fp);
}
void FileLogger::warningf(std::string filename, int line, const char* format,
                          ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[WARNING] %s %s:%d ", getCurrentTime().c_str(),
            filename.c_str(), line);
    vfprintf(fp, format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
    fflush(fp);
}
void FileLogger::errorf(std::string filename, int line, const char* format,
                        ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[ERROR]   %s %s:%d ", getCurrentTime().c_str(),
            filename.c_str(), line);
    vfprintf(fp, format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
    fflush(fp);
}
