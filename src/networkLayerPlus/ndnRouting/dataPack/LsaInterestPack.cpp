#include "LsaInterestPack.h"
using namespace std;
using json = nlohmann::json;

struct LsaInterestPacket {
    RouterID routerID;
    uint16_t lsType;
    int32_t sequenceNum;
} __attribute__((__packed__));

void LsaInterestPack::decode(const char* data, int dataLength) {
    const LsaInterestPacket* ptr = (const LsaInterestPacket*)data;
    routerID = ntoh(ptr->routerID);
    sequenceNum = ntoh(ptr->sequenceNum);
    lsType = (LinkStateType)(ntoh(ptr->lsType));
}
std::pair<int, std::unique_ptr<char[]>> LsaInterestPack::encode() {
    LsaInterestPacket packet;
    packet.routerID = hton(routerID);
    packet.lsType = hton((uint16_t)(lsType));
    packet.sequenceNum = hton(sequenceNum);
    char* buffer = new char[sizeof(LsaInterestPack)];
    memcpy(buffer, &packet, sizeof(LsaInterestPack));
    return {sizeof(LsaInterestPack), unique_ptr<char[]>(buffer)};
}

json LsaInterestPack::marshal() const {
    json j;
    j["routerID"] = routerID;
    j["lsType"] = lsType;
    j["sequenceNum"] = sequenceNum;

    return j;
}