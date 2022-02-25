#include "RegisterInterestPack.h"

using namespace std;
using nlohmann::json;

struct RegisterInterestInnerPack {
    RouterID root;
    uint16_t linkStateType;
    int32_t sequenceNum;
} __attribute__((__packed__));

void RegisterInterestPack::decode(const char* data, int dataLength) {
    const RegisterInterestInnerPack* ptr = (const RegisterInterestInnerPack*)(data);
    root = ntoh(ptr->root);
    linkStateType = (LinkStateType)ntoh(ptr->linkStateType);
    sequenceNum = ntoh(ptr->sequenceNum);
}

pair<int, std::unique_ptr<char[]>> RegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = sizeof(RegisterInterestInnerPack);
    char* buffer = new char[size];
    RegisterInterestInnerPack* ptr = (RegisterInterestInnerPack*)(buffer);
    ptr->root = hton(root);
    ptr->linkStateType = (LinkStateType)hton((uint16_t)linkStateType);
    ptr->sequenceNum = hton(sequenceNum);
    return {size, unique_ptr<char[]>(buffer)};
}

json RegisterInterestPack::marshal() const {
    json j;
    j["root"] = root;
    j["linkStateType"] = getNameForLinkStateType(linkStateType);
    j["sequenceNumber"] = sequenceNum;
    return j;
}