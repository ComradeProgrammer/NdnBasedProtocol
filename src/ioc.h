#ifndef __IOC_H_
#define __IOC_H_
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "linkLayer/transmitter/Transmitter.h"
#include "networkLayer/ip/routingTable/RoutingTable.h"
#include "physicalLayer/nic/NicManager.h"
#include "util/log/Logger.h"
#include "util/timer/Timer.h"
const std::string LOGGER_TYPE = "logger_type";
const std::string LOGGER_FILE = "logger_file";
const std::string LOGGER_TERMINAL = "logger_terminal";

const std::string LOGGER_FILENAME = "logger_filename";

const std::string DISPLAY_NAME = "display_name";

const std::string PLATFORM = "platform";
const std::string PLATFORM_UNIX = "platform_unix";

// actually Ioc serve as a collections of the singleton patterns
class Ioc {
   public:
    static Ioc* getIoc();
    /**
     * @brief actually Ioc serve as a collections of the singleton patterns, and the paramter configuration is the config key-value pairs. This function must be called at the beginning of the program, because it won't be locked and therefore not thread-safe. The following is the
     * usage:
     *
     * @param LOGGER_TYPE LOGGER_FILE / LOGGER_TERMINAL
     * @param LOGGER_FILENAME output logfile name, only valid if LOGGER_FILE is used
     * @param PLATFORM PLATFORM_UNIX or some other platforms
     * @param DISPLAY_NAME display name, used for output
     */
    static void IOCInit(std::unordered_map<std::string, std::string> configuration);

   private:
    static std::mutex staticLock;
    static Ioc* ioc;
    Ioc();

   public:
    std::shared_ptr<Logger> getLogger();
    std::shared_ptr<Timer> getTimer();
    std::shared_ptr<NicManager> getNicManager();
    std::shared_ptr<Transmitter> getTransmitter();
    std::shared_ptr<RoutingTable>getRoutingTable(){return ipRoutingTable;}
    std::string getDisplayName() { return displayName; }

   private:
    std::mutex lock;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<NicManager> nicManager;
    std::shared_ptr<Transmitter> transmitter;
    std::string displayName;
    std::shared_ptr<RoutingTable>ipRoutingTable;
};
#define LOGGER Ioc::getIoc()->getLogger()
#define IOC Ioc::getIoc()
#endif