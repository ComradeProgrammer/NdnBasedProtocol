#include "PitEntry.h"
using nlohmann::json;
using namespace std;
bool PitEntry::isLoopingInterest(int interfaceID, uint32_t nonce) {
    // std::lock_guard<std::mutex> lockFunction(lock);
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
    // std::lock_guard<std::mutex> lockFunction(lock);
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

vector<uint32_t> PitEntry::getAllNonce() {
    vector<uint32_t> res;
    for (int i = 0; i < inRecords.size(); i++) {
        for (uint32_t nonce : inRecords[i]->getNonceList()) {
            res.push_back(nonce);
        }
    }
    return res;
}
std::vector<int> PitEntry::getAllPendingInterfaces() {
    vector<int> res;
    for (int i = 0; i < inRecords.size(); i++) {
        res.push_back(inRecords[i]->getInterfaceID());
    }
    return res;
}

string PitEntry::toString() const {
    vector<std::string> res;
    for (auto ptr : inRecords) {
        res.push_back(ptr->toString());
    }
    json j(res);
    return j.dump();
}
