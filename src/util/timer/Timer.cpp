#include "Timer.h"
using namespace std;

void Timer::startTimer(string name, int duration,
                       function<bool(string)> callback) {
    lock_guard<mutex> lockBlock(lock);

    if (threads.find(name) != threads.end()) {
        logger->WARNING("Timer::startTimer : timer " + name + " exists");
        return;
    }
    thread newTimer([this, name, duration]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        onTimerUp(name);
    });
    threads[name] = newTimer.get_id();
    callbacks[name] = callback;
    durations[name] = duration;
    newTimer.detach();
}

void Timer::cancelTimer(std::string name) {
    lock_guard<mutex> lockBlock(lock);
    if (threads.find(name) == threads.end()) {
        logger->WARNING("Timer::cancelTimer : timer " + name +
                        " does not exist");
        return;
    }
    callbacks.erase(callbacks.find(name));
    threads.erase(threads.find(name));
    durations.erase(durations.find(name));
}

void Timer::onTimerUp(std::string name) {
    lock.lock();
    auto iterator = threads.find(name);
    if (iterator == threads.end() ||
        iterator->second != std::this_thread::get_id()) {
        logger->INFO("Timer::onTimerUp : timer " + name + " has been canceled");
        lock.unlock();
        return;
    }
    auto callback = callbacks[name];
    int duration = durations[name];

    callbacks.erase(callbacks.find(name));
    threads.erase(threads.find(name));
    durations.erase(durations.find(name));
    lock.unlock();

    bool refresh = callback(name);
    if (refresh) {
        startTimer(name, duration, callback);
    }
}