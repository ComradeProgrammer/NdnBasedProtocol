#include "FileLogger.h"
using namespace std;
FileLogger::FileLogger(std::string logName) {
    fp = fopen(logName.c_str(), "w+");
}

FileLogger::~FileLogger() { fclose(fp); }

void FileLogger::verbose(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    printf("[VERBOSE] %s:%d %s\n", filename.c_str(), line, s.c_str());
}

void FileLogger::info(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    printf("[INFO   ] %s:%d %s\n", filename.c_str(), line, s.c_str());
}
void FileLogger::warning(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    printf("[WARNING] %s:%d %s\n", filename.c_str(), line, s.c_str());
}
void FileLogger::error(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    printf("[ERROR  ] %s:%d %s\n", filename.c_str(), line, s.c_str());
}

void FileLogger::verbosef(std::string filename, int line, const char* format,
                          ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[VERBOSE] %s:%d ", filename.c_str(), line);
    vprintf(format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
}
void FileLogger::infof(std::string filename, int line, const char* format,
                       ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[INFO   ] %s:%d ", filename.c_str(), line);
    vprintf(format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
}
void FileLogger::warningf(std::string filename, int line, const char* format,
                          ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[WARNING] %s:%d ", filename.c_str(), line);
    vprintf(format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
}
void FileLogger::errorf(std::string filename, int line, const char* format,
                        ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    fprintf(fp, "[ERROR  ] %s:%d ", filename.c_str(), line);
    vprintf(format, args_list);
    fprintf(fp, "\n");
    va_end(args_list);
}
