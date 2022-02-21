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
/**
 * @brief A Timer object is used to manage all Timer events. This Timer object
 * is implemented via user-space thread library pthread. THREAD SAFE.
 */
class Timer {
   public:
    Timer() = default;

   private:
    // constructor
    // prohibited to copy
    Timer(const Timer&) = delete;

   public:
    /**
     * @brief start a timer at once. The timer will call the callback function
     * once the time is up
     * @param name UNIQUE identifier for this timer.
     * @param duration duration for the timer, unit is MILLISECONDS
     * @param callback std::function<bool(std::string)> callback. parameter
     * string is the name of the due timer, and the returned boolean value means
     * whether this timer should be restarted.
     */
    void startTimer(std::string name, int duration, std::function<bool(std::string)> callback);
    /**
     * @brief cancel a timer
     * @param name the name of the timer you want to cancel
     */
    void cancelTimer(std::string name);

   private:
    std::unordered_map<std::string, std::function<bool(std::string)>> callbacks;
    std::unordered_map<std::string, std::thread::id> threads;
    std::unordered_map<std::string, int> durations;
    std::mutex lock;

    void onTimerUp(std::string name);
};

#endif