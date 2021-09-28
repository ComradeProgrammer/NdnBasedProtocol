#ifndef __PITENTRY_H_
#define __PITENTRY_H_
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "PitInRecord.h"
// NOT THREAD SAFE class PitEntry;
class PitEntry {
   public:
    PitEntry(std::string _name) : name(_name) {}
    virtual ~PitEntry() = default;

    std::string getName() {
        // std::lock_guard<std::mutex> lockFunction(lock);
        return name;
    }

    void addInputRecord(int interfaceID, uint32_t nonce);
    bool isLoopingInterest(int interfaceID, uint32_t nonce);
    bool isPending() {
        // std::lock_guard<std::mutex> lockFunction(lock);
        return inRecords.size() != 0;
    }

    std::string getTimerName() {
        return std::string("PitEntry::expire::") + name;
    }
    std::vector<uint32_t> getAllNonce();
    std::vector<int> getAllPendingInterfaces();
    std::string toString() const;

   private:
    std::string name;
    std::vector<std::shared_ptr<PitInRecord>> inRecords;
    // std::mutex lock;
};

#endif