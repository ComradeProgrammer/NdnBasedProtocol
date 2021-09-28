#ifndef __DEADNONCELIST_H_
#define __DEADNONCELIST_H_
#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "util/cityhash/cityhash.h"
#include "util/log/Logger.h"
#include "util/timer/Timer.h"
/**
 *@brief DeadNonceList class. THREAD SAFE
 */
class DeadNonceList {
   public:
    // default constructor
    DeadNonceList(std::shared_ptr<Logger> log = nullptr);
    virtual ~DeadNonceList() = default;

    /**
     * @brief add a name and nonce to the deadnoncelist
     * @param name ndn packet name
     * @param nonce ndn packet nonce
     */
    void addToDeadNonceList(std::string name, uint32_t nonce);

    /**
     * @brief check wherther a package exist in deadnoncelist according to the
     * name and nonce
     * @param name ndn packet name
     * @param nonce ndn packet nonce
     * @return boolean value: whether this package is found in deadnoncelist,
     * return true if found
     */
    bool isInDeadNonceList(std::string name, uint32_t nonce);

    /**
     * @brief callback function when timer is triggered. This function will
     * insert the mark and reevaluate the proper size of deadnoncelist. This
     * function is register to the timer via a lambda expression in constructor
     */
    void onTimerTriggered();

   private:
    // how long an entry is expected to be alive. This value will influence the
    // erasing rate of deadnonce item
    int entryLifeTime = 6;
    // maximum limit of deadnoncelist
    int sizeLimit = 100;
    // capacity-change timer interval, unit: milisecond
    int interval = 10000;
    // how many marks currently is in the list
    int markNumber = 0;
    /**
     * @brief store the the deadNonce. Two kinds of pair will exist, the first
     * kind is the real dead nonce, the second kind is a special mark used to
     * implement the dynamic change of the deadnonce capacity erasing rate.
     *
     * 1st value of the pair is a 64-bit integer, where upper 32 bits are hash
     * value of the name of the packet, and the lower 32 bits are the nonce. 2nd
     * value is a boolean marking whether is a mark
     *
     */
    std::list<std::pair<uint64_t, bool>> deadNonceList;
    std::shared_ptr<Logger> logger;
    std::mutex lock;
    void insertToList(uint64_t value, bool isMark);
};
#endif