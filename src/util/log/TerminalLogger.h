#ifndef __TERMINAL_LOGGER_H_
#define __TERMINAL_LOGGER_H_

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "Logger.h"
#include "util/util.h"
// THREAD SAFE colorful Logger
class TerminalLogger : public Logger {
   public:
    // when default logger is used(this class), output will be stdout
    TerminalLogger() = default;
    TerminalLogger(const TerminalLogger&) = delete;
    virtual ~TerminalLogger() = default;

    virtual void verbose(std::string filename, int line, std::string s) override;
    virtual void info(std::string filename, int line, int level, std::string s) override;
    virtual void warning(std::string filename, int line, std::string s) override;
    virtual void error(std::string filename, int line, std::string s) override;

    virtual void verbosef(std::string filename, int line, const char* format, ...) override;
    virtual void infof(std::string filename, int line, int level, const char* format, ...) override;
    virtual void warningf(std::string filename, int line, const char* format, ...) override;
    virtual void errorf(std::string filename, int line, const char* format, ...) override;

    void setLevels(std::vector<int> _levels) override;

   protected:
    std::mutex lock;
    std::vector<int> levels;
};

#endif
