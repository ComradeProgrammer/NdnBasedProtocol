#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <mutex>

//THREAD SAFE colorful Logger
class Logger {
   public:
   //when default logger is used(this class), output will be stdout
    Logger() = default;
    Logger(const Logger&) = delete;
    virtual ~Logger() = default;

    virtual void verbose(std::string filename, int line, std::string s);
    virtual void info(std::string filename, int line, std::string s);
    virtual void warning(std::string filename, int line, std::string s);
    virtual void error(std::string filename, int line, std::string s);

    static std::shared_ptr<Logger> getDefaultLoggerIfNull(
        std::shared_ptr<Logger> log);
    
    protected:
    std::mutex lock;

};

#define VERBOSE(s) verbose(__FILE__, __LINE__, s)
#define INFO(s) info(__FILE__, __LINE__, s)
#define WARNING(s) warning(__FILE__, __LINE__, s)
#define ERROR(s) error(__FILE__, __LINE__, s)

#endif
