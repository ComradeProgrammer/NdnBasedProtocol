#ifndef __IOC_H_
#define __IOC_H_
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

const std::string LOGGER_TYPE = "logger_type";
const std::string LOGGER_FILE = "logger_file";
const std::string LOGGER_TERMINAL = "logger_terminal";
const std::string LOGGER_FILENAME = "logger_filename";
const std::string PLATFORM = "platform";
const std::string PLATFORM_UNIX = "platform_unix";

class Logger;
class Timer;
class NicManager;
class Transmitter;
class Ioc {
   public:
    static Ioc* getIoc();
    static void IOCInit(std::unordered_map<std::string, std::string> configuration);

   private:
    static std::mutex staticLock;
    static Ioc* ioc;
    Ioc(std::unordered_map<std::string, std::string> configuration);

   public:
    std::shared_ptr<Logger> getLogger();
    std::shared_ptr<Timer> getTimer();
    std::shared_ptr<NicManager> getNicManager();
    std::shared_ptr<Transmitter>getTransmitter();

   private:
    std::mutex lock;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<NicManager> nicManager;
    std::shared_ptr<Transmitter>transmitter;
};
#define LOGGER Ioc::getIoc()->getLogger()
#define IOC Ioc::getIoc()
#endif