#ifndef __PITINPUTRECORD_H_
#define __PITINPUTRECORD_H_
#include <cstdint>
#include <mutex>
#include <set>
// Thread safe class PitRecord
class PitInRecord {
   public:
    PitInRecord(int _interfaceID) : interfaceID(_interfaceID) {}

    int getInterfaceID() {
        std::lock_guard<std::mutex> lockFunction(lock);
        return interfaceID;
    }
    bool findNonce(uint32_t nonce) {
        std::lock_guard<std::mutex> lockFunction(lock);
        return nonceList.find(nonce) != nonceList.end();
    }
    void addNonce(uint32_t nonce) {
        std::lock_guard<std::mutex> lockFunction(lock);
        nonceList.insert(nonce);
    }

   private:
    int interfaceID;
    std::set<uint32_t> nonceList;
    std::mutex lock;
};
#endif