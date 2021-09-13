#ifndef __FILELOGGER_H_
#define __FILELOGGER_H_
#include <fstream>

#include "Logger.h"

class FileLogger : public Logger {
   public:
    FileLogger(std::string logName);
    FileLogger(const FileLogger&) = delete;
    virtual ~FileLogger();

    virtual void verbose(std::string filename, int line, std::string s) override;
    virtual void info(std::string filename, int line, std::string s) override;
    virtual void warning(std::string filename, int line, std::string s) override;
    virtual void error(std::string filename, int line, std::string s) override;

    virtual void verbosef(std::string filename, int line, const char* format, ...) override;
    virtual void infof(std::string filename, int line, const char* format, ...)override;
    virtual void warningf(std::string filename, int line, const char* format, ...)override;
    virtual void errorf(std::string filename, int line, const char* format, ...)override;

   private:
    std::fstream out;
    FILE * fp;
};

#endif
