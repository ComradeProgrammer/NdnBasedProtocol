#include "RegisterInterestPack.h"

using namespace std;
using nlohmann::json;

struct RegisterInterestInnerPack {
    //RouterID root;
    int32_t rchSequencesNum;
    int32_t adjSequencesNum;
    time_t timestamp;

} __attribute__((__packed__));

void RegisterInterestPack::decode(const char* data, int dataLength) {
    const RegisterInterestInnerPack* ptr = (const RegisterInterestInnerPack*)(data);
    //root = ntoh(ptr->root);
    rchSequenceNum = ntoh(ptr->rchSequencesNum);
    adjSequenceNum = ntoh(ptr->adjSequencesNum);
    timestamp=ntoh(ptr->timestamp);
}

pair<int, std::unique_ptr<char[]>> RegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = sizeof(RegisterInterestInnerPack);
    char* buffer = new char[size];
    RegisterInterestInnerPack* ptr = (RegisterInterestInnerPack*)(buffer);
    //ptr->root = hton(root);
    ptr->rchSequencesNum = hton(rchSequenceNum);
    ptr->adjSequencesNum = hton(adjSequenceNum);
    ptr->timestamp=hton(timestamp);
    return {size, unique_ptr<char[]>(buffer)};
}

json RegisterInterestPack::marshal() const {
    json j;
    //j["root"] = root;
    j["adjSequenceNumber"] = adjSequenceNum;
    j["rchSequenceNumber"] = rchSequenceNum;
    j["timestamp"]=timestamp;
    return j;
}