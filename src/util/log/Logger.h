#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <fstream>
#include <iostream>
#include <string>

class Logger {
   public:
    Logger() = default;
    Logger(const Logger&) = delete;
    virtual ~Logger() = default;

    virtual void verbose(std::string filename, int line, std::string s);
    virtual void info(std::string filename, int line, std::string s);
    virtual void warning(std::string filename, int line, std::string s);
    virtual void error(std::string filename, int line, std::string s);
};

#define VERBOSE(s) verbose(__FILE__, __LINE__, s)
#define INFO(s) info(__FILE__, __LINE__, s)
#define WARNING(s) warning(__FILE__, __LINE__, s)
#define ERROR(s) error(__FILE__, __LINE__, s)

#endif
