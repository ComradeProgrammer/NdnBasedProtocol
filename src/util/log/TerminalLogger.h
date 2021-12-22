#ifndef __TERMINAL_LOGGER_H_
#define __TERMINAL_LOGGER_H_

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "util/util.h"
#include"Logger.h"
// THREAD SAFE colorful Logger
class TerminalLogger : public Logger{
   public:
    // when default logger is used(this class), output will be stdout
    TerminalLogger() = default;
    TerminalLogger(const TerminalLogger&) = delete;
    virtual ~TerminalLogger() = default;

    virtual void verbose(std::string filename, int line, std::string s);
    virtual void info(std::string filename, int line, std::string s);
    virtual void warning(std::string filename, int line, std::string s);
    virtual void error(std::string filename, int line, std::string s);

    virtual void verbosef(std::string filename, int line, const char* format, ...);
    virtual void infof(std::string filename, int line, const char* format, ...);
    virtual void warningf(std::string filename, int line, const char* format, ...);
    virtual void errorf(std::string filename, int line, const char* format, ...);

   protected:
    std::mutex lock;
};

#endif
