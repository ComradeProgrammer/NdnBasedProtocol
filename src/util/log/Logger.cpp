#include "Logger.h"
using namespace std;

void Logger::verbose(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[34m" << getCurrentTime() << " " << filename << ":" << line
         << " " << s << "\033[0m" << endl;  // blue
}

void Logger::info(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[32m" << getCurrentTime() << filename << ":" << line << " "
         << s << "\033[0m" << endl;  // green
}
void Logger::warning(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[33m" << getCurrentTime() << filename << ":" << line << " "
         << s << "\033[0m" << endl;  // yellow
}
void Logger::error(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[31m" << getCurrentTime() << filename << ":" << line << " "
         << s << "\033[0m" << endl;  // red
}

void Logger::verbosef(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);

    va_list args_list;
    va_start(args_list, format);
    printf("\033[34m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(),
           line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void Logger::infof(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    printf("\033[32m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(),
           line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void Logger::warningf(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    printf("\033[33m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(),
           line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void Logger::errorf(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    printf("\033[31m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(),
           line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}

std::shared_ptr<Logger> Logger::getDefaultLoggerIfNull(
    std::shared_ptr<Logger> log) {
    if (log == nullptr) {
        return make_shared<Logger>();
    }
    return log;
}
