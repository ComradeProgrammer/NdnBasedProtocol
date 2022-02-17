#ifndef __IOC_H_
#define __IOC_H_
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include"util/log/Logger.h"
#include"util/timer/Timer.h"
#include"physicalLayer/nic/NicManager.h"
#include"linkLayer/transmitter/Transmitter.h"
const std::string LOGGER_TYPE = "logger_type";
const std::string LOGGER_FILE = "logger_file";
const std::string LOGGER_TERMINAL = "logger_terminal";

const std::string LOGGER_FILENAME = "logger_filename";

const std::string DISPLAY_NAME = "display_name";

const std::string PLATFORM = "platform";
const std::string PLATFORM_UNIX = "platform_unix";


//actually Ioc serve as a collections of the singleton patterns
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
    std::string getDisplayName(){return displayName;}

   private:
    std::mutex lock;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<NicManager> nicManager;
    std::shared_ptr<Transmitter>transmitter;
    std::string displayName;
};
#define LOGGER Ioc::getIoc()->getLogger()
#define IOC Ioc::getIoc()
#endif