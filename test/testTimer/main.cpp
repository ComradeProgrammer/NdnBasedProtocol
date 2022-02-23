#include <unistd.h>

#include <memory>

#include "ioc.h"
#include "util/log/FileLogger.h"
#include "util/log/Logger.h"
#include "util/log/TerminalLogger.h"
#include "util/timer/Timer.h"
using namespace std;
int main() {
    Ioc::IOCInit({
        {LOGGER_TYPE, LOGGER_TERMINAL},
        {PLATFORM, PLATFORM_UNIX},
    });
    auto timer = IOC->getTimer();
    timer->startTimer("test", 500, [](string) -> bool {
        LOGGER->INFO(0,"here");
        return true;
    });
    while (1) {
        sleep(1);
    }
}