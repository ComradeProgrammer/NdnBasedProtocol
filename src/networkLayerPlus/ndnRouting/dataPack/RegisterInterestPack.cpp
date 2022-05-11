#include "RegisterInterestPack.h"

using namespace std;
using nlohmann::json;

struct RegisterInterestInnerPack {
    RouterID root;
    int32_t rchSequencesNum;
    int32_t adjSequencesNum;

} __attribute__((__packed__));

void RegisterInterestPack::decode(const char* data, int dataLength) {
    const RegisterInterestInnerPack* ptr = (const RegisterInterestInnerPack*)(data);
    root = ntoh(ptr->root);
    rchSequenceNum = ntoh(ptr->rchSequencesNum);
    adjSequenceNum = ntoh(ptr->adjSequencesNum);
}

pair<int, std::unique_ptr<char[]>> RegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = sizeof(RegisterInterestInnerPack);
    char* buffer = new char[size];
    RegisterInterestInnerPack* ptr = (RegisterInterestInnerPack*)(buffer);
    ptr->root = hton(root);
    ptr->rchSequencesNum = hton(rchSequenceNum);
    ptr->adjSequencesNum = hton(adjSequenceNum);

    return {size, unique_ptr<char[]>(buffer)};
}

json RegisterInterestPack::marshal() const {
    json j;
    j["root"] = root;
    j["adjSequenceNumber"] = adjSequenceNum;
    j["rchSequenceNumber"] = rchSequenceNum;
    return j;
}