#include "PitEntry.h"

using namespace std;
bool PitEntry::isLoopingInterest(int interfaceID, uint32_t nonce) {
    //std::lock_guard<std::mutex> lockFunction(lock);
    for (auto ptr : inRecords) {
        if (interfaceID == ptr->getInterfaceID()) {
            continue;
        }
        if (ptr->findNonce(nonce)) {
            return true;
        }
    }
    return false;
}

void PitEntry::addInputRecord(int interfaceID, uint32_t nonce) {
    //std::lock_guard<std::mutex> lockFunction(lock);
    for (auto ptr : inRecords) {
        if (ptr->getInterfaceID() == interfaceID) {
            ptr->addNonce(nonce);
            return;
        }
    }
    auto newRecord = make_shared<PitInRecord>(interfaceID);
    newRecord->addNonce(nonce);
    inRecords.push_back(newRecord);
}