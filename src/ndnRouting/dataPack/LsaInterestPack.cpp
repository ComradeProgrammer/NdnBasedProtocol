#include "LsaInterestPack.h"
using namespace std;
using json = nlohmann::json;

struct LsaInterestPacket {
    uint32_t routerID;
    uint16_t lsType;
    int32_t sequenceNum;
} __attribute__((__packed__));

void LsaInterestPack::decode(const char* data, int dataLength) {
    const LsaInterestPacket* ptr = (const LsaInterestPacket*)data;
    routerID = ntohl(ptr->routerID);
    sequenceNum = ntohl(ptr->sequenceNum);
    lsType = (LinkStateType)(ntohs(ptr->lsType));
}
std::pair<int, std::unique_ptr<char[]>> LsaInterestPack::encode() {
    LsaInterestPacket packet;
    packet.routerID = htonl(routerID);
    packet.lsType = htons((uint16_t)(lsType));
    packet.sequenceNum = htonl(sequenceNum);
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