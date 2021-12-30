#ifndef __LOGGER_H_
#define __LOGGER_H_
#include <string>
#include <mutex>
#include <memory>
class Logger {
   public:
    virtual void verbose(std::string filename, int line, std::string s)=0;
    virtual void info(std::string filename, int line, std::string s)=0;
    virtual void warning(std::string filename, int line, std::string s)=0;
    virtual void error(std::string filename, int line, std::string s)=0;

    virtual void verbosef(std::string filename, int line, const char* format, ...)=0;
    virtual void infof(std::string filename, int line, const char* format, ...)=0;
    virtual void warningf(std::string filename, int line, const char* format, ...)=0;
    virtual void errorf(std::string filename, int line, const char* format, ...)=0;
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