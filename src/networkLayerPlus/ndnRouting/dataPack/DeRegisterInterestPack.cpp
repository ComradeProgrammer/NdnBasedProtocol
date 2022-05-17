#include "DeRegisterInterestPack.h"
using namespace std;
using nlohmann::json;

struct DeRegisterInterestInnerPack {
    RouterID root;
    time_t timestamp;

} __attribute__((__packed__));

void DeRegisterInterestPack::decode(const char* data, int dataLength) {
    const DeRegisterInterestInnerPack* ptr = (const DeRegisterInterestInnerPack*)(data);
    root = ntoh(ptr->root);
    timestamp=ntoh(ptr->timestamp);

}

pair<int, std::unique_ptr<char[]>> DeRegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = sizeof(DeRegisterInterestInnerPack);
    char* buffer = new char[size];
    DeRegisterInterestInnerPack* ptr = (DeRegisterInterestInnerPack*)(buffer);
    ptr->root = hton(root);
    ptr->timestamp=hton(timestamp);
    return {size, unique_ptr<char[]>(buffer)};
}

json DeRegisterInterestPack::marshal() const {
    json j;
    j["root"] = root;
    return j;
}
