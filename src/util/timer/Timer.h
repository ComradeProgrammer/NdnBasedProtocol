#ifndef __TIMER_H_
#define __TIMER_H_
#include <time.h>

#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "util/log/Logger.h"
class Timer {
   public:
    Timer(std::shared_ptr<Logger> log = nullptr) {
        logger = Logger::getDefaultLoggerIfNull(log);
    };

    Timer(const Timer&) = delete;

    void startTimer(std::string name, int duration,
                    std::function<bool(std::string)> callback);
    void cancelTimer(std::string name);

   private:
    std::shared_ptr<Logger> logger = nullptr;
    std::unordered_map<std::string, std::function<bool(std::string)>> callbacks;
    std::unordered_map<std::string, std::thread::id> threads;
    std::unordered_map<std::string, int> durations;
    std::mutex lock;

    void onTimerUp(std::string name);
};

#endif