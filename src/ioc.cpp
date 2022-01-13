#include "ioc.h"

#include "physicalLayer/nic/NicManager.h"
#include "physicalLayer/nic/NicManagerUnix.h"
#include "util/log/FileLogger.h"
#include "util/log/Logger.h"
#include "util/log/TerminalLogger.h"
#include "util/timer/Timer.h"
#include "linkLayer/transmitter/Transmitter.h"
#include "linkLayer/transmitter/RawSocketTransmitter.h"

using namespace std;

Ioc* Ioc::ioc = nullptr;
mutex Ioc::staticLock;

Ioc* Ioc::getIoc() {
    lock_guard<mutex> _(staticLock);
    return ioc;
}

void Ioc::IOCInit(unordered_map<string, string> configuration) {
    lock_guard<mutex> _(staticLock);
    ioc = new Ioc(configuration);
}

Ioc::Ioc(unordered_map<string, string> configuration) {
    string loggerType = configuration[LOGGER_TYPE];
    if (loggerType == LOGGER_FILE) {
        logger = make_shared<FileLogger>(configuration[LOGGER_FILENAME]);
    } else if (loggerType == LOGGER_TERMINAL) {
        logger = make_shared<TerminalLogger>();
    }

    timer = make_shared<Timer>();

    string platform = configuration[PLATFORM];
    if (platform == PLATFORM_UNIX) {
        nicManager = make_shared<NicManagerUnix>();
        transmitter=make_shared<RawSocketTransmitter>();
    }
}

shared_ptr<Logger> Ioc::getLogger() {
    lock_guard<mutex> _(lock);
    return logger;
}
shared_ptr<Timer> Ioc::getTimer() {
    lock_guard<mutex> _(lock);
    return timer;
}

shared_ptr<NicManager> Ioc::getNicManager() {
    lock_guard<mutex> _(lock);
    return nicManager;
}
shared_ptr<Transmitter>Ioc::getTransmitter(){
    lock_guard<mutex> _(lock);
    return transmitter;
}