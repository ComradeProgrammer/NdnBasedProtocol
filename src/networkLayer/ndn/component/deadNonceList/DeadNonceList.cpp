#include "DeadNonceList.h"
using namespace std;
DeadNonceList::DeadNonceList(std::shared_ptr<Logger> log) {
    // create a timing event
    shared_ptr<Timer> timer = IOC->getTimer();
    timer->startTimer("deadnoncelist-capicity", interval, [this](string) -> bool {
        LOGGER->INFO("DeadNonceList::resize");
        onTimerTriggered();
        return true;  // auto reset
    });
}

void DeadNonceList::addToDeadNonceList(string name, uint32_t nonce) {
    lock_guard<mutex> lockFunction(lock);
    uint64_t nameHash = ::CityHash32(name.c_str(), name.size() + 1);
    uint64_t hash = ((nameHash << 32) | nonce);
    insertToList(hash, false);
}

bool DeadNonceList::isInDeadNonceList(string name, uint32_t nonce) {
    lock_guard<mutex> lockFunction(lock);
    uint64_t nameHash = ::CityHash32(name.c_str(), name.size() + 1);
    uint64_t hash = ((nameHash << 32) | nonce);
    auto itr = std::find(deadNonceList.begin(), deadNonceList.end(), pair<uint64_t, bool>(hash, false));
    if (itr == deadNonceList.end()) {
        return false;
    }
    return true;
}

void DeadNonceList::onTimerTriggered() {
    lock_guard<mutex> lockFunction(lock);
    int expectedMark = entryLifeTime / interval;
    if (expectedMark > markNumber) {
        // increase the size;
        sizeLimit = sizeLimit * 1.2;
    } else if (expectedMark < markNumber) {
        // decrease the size;
        sizeLimit = sizeLimit * 0.9;
        // prevent the size reaches 0
        if (sizeLimit == 0) {
            sizeLimit = 1;
        }
    }
    // insert a mark;
    insertToList(0, true);
}

void DeadNonceList::insertToList(uint64_t value, bool isMark) {
    deadNonceList.emplace_back(pair<uint64_t, bool>(value, isMark));
    if (isMark) {
        markNumber++;
    }
    while (deadNonceList.size() > sizeLimit) {
        auto tmp = deadNonceList.front();
        deadNonceList.pop_front();
        if (tmp.second) {
            markNumber--;
        }
    }
}
