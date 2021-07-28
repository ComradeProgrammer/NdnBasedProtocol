#include "Logger.h"
using namespace std;


void Logger::verbose(std::string filename, int line, std::string s) {
  cout << "\033[34m" << filename << ":" << line << " " << s << "\033[0m"
      << endl;  // blue
}

void Logger::info(std::string filename, int line, std::string s) {
  cout << "\033[32m" << filename << ":" << line << " " << s << "\033[0m"
       << endl;  // green
}
void Logger::warning(std::string filename, int line, std::string s) {
  cout << "\033[33m" << filename << ":" << line << " " << s << "\033[0m"
       << endl;  // yellow
}
void Logger::error(std::string filename, int line, std::string s) {
  cout << "\033[31m" << filename << ":" << line << " " << s << "\033[0m"
       << endl;  // red
}
