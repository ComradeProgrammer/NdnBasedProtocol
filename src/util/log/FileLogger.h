#ifndef __FILELOGGER_H_
#define __FILELOGGER_H_

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "Logger.h"
#include "util/util.h"

class FileLogger : public Logger {
   public:
    FileLogger(std::string logName);
    FileLogger(const FileLogger&) = delete;
    virtual ~FileLogger();

    virtual void verbose(std::string filename, int line, std::string s) override;
    virtual void info(std::string filename, int line,int level ,std::string s) override;
    virtual void warning(std::string filename, int line, std::string s) override;
    virtual void error(std::string filename, int line, std::string s) override;

    virtual void verbosef(std::string filename, int line, const char* format, ...) override;
    virtual void infof(std::string filename, int line,int level, const char* format, ...) override;
    virtual void warningf(std::string filename, int line, const char* format, ...) override;
    virtual void errorf(std::string filename, int line, const char* format, ...) override;

    void setLevels(std::vector<int> _levels)override;
   private:
    FILE* fp;
    std::mutex lock;
    std::vector<int> levels;

};

#endif
