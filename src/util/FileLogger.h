#ifndef __FILELOGGER_H_
#define __FILELOGGER_H_
#include<fstream>
#include"Logger.h"

class FileLogger:public Logger{
  public:
    FileLogger(std::string logName);
    FileLogger(const FileLogger& )=delete;
    ~FileLogger();

  void verbose(std::string filename, int line, std::string s)override;
  void info(std::string filename, int line, std::string s)override;
  void warning(std::string filename, int line, std::string s)override;
  void error(std::string filename, int line, std::string s)override;

  private:
    std::fstream out;
};

#endif
