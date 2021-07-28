#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <fstream>
#include <iostream>
#include <string>

class Logger {
 public:
  Logger()=default;
  Logger(const Logger&) = delete;
  virtual ~Logger() = default;

  virtual void verbose(std::string filename, int line, std::string s);
  virtual void info(std::string filename, int line, std::string s);
  virtual void warning(std::string filename, int line, std::string s);
  virtual void error(std::string filename, int line, std::string s);

};

#endif
