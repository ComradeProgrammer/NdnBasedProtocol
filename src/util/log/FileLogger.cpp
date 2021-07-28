#include "FileLogger.h"
using namespace std;
FileLogger::FileLogger(std::string logName) { out.open(logName, ios::out); }

FileLogger::~FileLogger() { out.close(); }

void FileLogger::verbose(std::string filename, int line, std::string s) {
    out << "[VERBOSE]" << filename << ":" << line << " " << s << endl;  // blue
}

void FileLogger::info(std::string filename, int line, std::string s) {
    out << "[INFO   ]" << filename << ":" << line << " " << s << endl;  // green
}
void FileLogger::warning(std::string filename, int line, std::string s) {
    out << "[wARNING]" << filename << ":" << line << " " << s
        << endl;  // yellow
}
void FileLogger::error(std::string filename, int line, std::string s) {
    out << "[ERROR  ]" << filename << ":" << line << " " << s << endl;  // red
}
