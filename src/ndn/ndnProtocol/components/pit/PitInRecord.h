#ifndef __PITINPUTRECORD_H_
#define __PITINPUTRECORD_H_
#include <cstdint>
#include <mutex>
#include <set>
#include"util/json/json.hpp"
// not Thread safe class PitRecord
class PitInRecord {
   public:
    PitInRecord(int _interfaceID) : interfaceID(_interfaceID) {}

    int getInterfaceID() {
        // std::lock_guard<std::mutex> lockFunction(lock);
        return interfaceID;
    }
    bool findNonce(uint32_t nonce) {
        // std::lock_guard<std::mutex> lockFunction(lock);
        return nonceList.find(nonce) != nonceList.end();
    }
    void addNonce(uint32_t nonce) {
        // std::lock_guard<std::mutex> lockFunction(lock);
        nonceList.insert(nonce);
    }
    std::string toString()const{
        nlohmann::json j;
        j["interfaceID"]=interfaceID;
        std::vector<int>res;
        for(auto i:nonceList){
            res.push_back(i);
        }
        j["nonceList"]=res;
        return j.dump();
    }

    std::set<uint32_t> getNonceList(){return nonceList;}

   private:
    int interfaceID;
    std::set<uint32_t> nonceList;
    // std::mutex lock;
};
#endif