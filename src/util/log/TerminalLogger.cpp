#include "TerminalLogger.h"
using namespace std;

void TerminalLogger::verbose(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[34m" << getCurrentTime() << " " << filename << ":" << line << " " << s << "\033[0m" << endl;  // blue
}

void TerminalLogger::info(std::string filename, int line, int level, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    bool found = false;
    for (auto i : levels) {
        if (level == i) {
            found = true;
            break;
        }
    }
    if (!found) {
        return;
    }
    cout << "\033[32m" << getCurrentTime() << filename << ":" << line << " " << s << "\033[0m" << endl;  // green
}
void TerminalLogger::warning(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[33m" << getCurrentTime() << filename << ":" << line << " " << s << "\033[0m" << endl;  // yellow
}
void TerminalLogger::error(std::string filename, int line, std::string s) {
    lock_guard<mutex> lockBlock(lock);
    cout << "\033[31m" << getCurrentTime() << filename << ":" << line << " " << s << "\033[0m" << endl;  // red
}

void TerminalLogger::verbosef(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);

    va_list args_list;
    va_start(args_list, format);
    printf("\033[34m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(), line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void TerminalLogger::infof(std::string filename, int line, int level, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);
    bool found = false;
    for (auto i : levels) {
        if (level == i) {
            found = true;
            break;
        }
    }
    if (!found) {
        return;
    }
    va_list args_list;
    va_start(args_list, format);
    printf("\033[32m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(), line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void TerminalLogger::warningf(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    printf("\033[33m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(), line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void TerminalLogger::errorf(std::string filename, int line, const char* format, ...) {
    lock_guard<mutex> lockBlock(lock);
    va_list args_list;
    va_start(args_list, format);
    printf("\033[31m %s %s:%d ", getCurrentTime().c_str(), filename.c_str(), line);
    vprintf(format, args_list);
    printf("\n\033[0m");
    va_end(args_list);
}
void TerminalLogger::setLevels(std::vector<int> _levels) {
    lock_guard<mutex> lockBlock(lock);
    levels = _levels;
}
