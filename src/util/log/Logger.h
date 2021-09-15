#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <cstdarg>
#include <string>
#include"util/util.h"
// THREAD SAFE colorful Logger
class Logger {
   public:
    // when default logger is used(this class), output will be stdout
    Logger() = default;
    Logger(const Logger&) = delete;
    virtual ~Logger() = default;

    virtual void verbose(std::string filename, int line, std::string s);
    virtual void info(std::string filename, int line, std::string s);
    virtual void warning(std::string filename, int line, std::string s);
    virtual void error(std::string filename, int line, std::string s);

    virtual void verbosef(std::string filename, int line, const char* format,
                          ...);
    virtual void infof(std::string filename, int line, const char* format,
                      ...);
    virtual void warningf(std::string filename, int line, const char* format,
                          ...);
    virtual void errorf(std::string filename, int line, const char* format,
                        ...);

    static std::shared_ptr<Logger> getDefaultLoggerIfNull(
        std::shared_ptr<Logger> log);

   protected:
    std::mutex lock;
};

#define VERBOSE(s) verbose(__FILE__, __LINE__, s)
#define INFO(s) info(__FILE__, __LINE__, s)
#define WARNING(s) warning(__FILE__, __LINE__, s)
#define ERROR(s) error(__FILE__, __LINE__, s)

#define VERBOSEF(format, ...) verbosef(__FILE__, __LINE__, format, __VA_ARGS__)
#define INFOF(format, ...) infof(__FILE__, __LINE__, format, __VA_ARGS__)
#define WARNINGF(format, ...) warningf(__FILE__, __LINE__, format, __VA_ARGS__)
#define ERRORF(format, ...) errorf(__FILE__, __LINE__, format, __VA_ARGS__)
#endif
