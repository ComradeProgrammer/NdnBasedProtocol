#include "RegisterDataPack.h"
using namespace std;
using json = nlohmann::json;

struct RegisterDataPackHeader {
    uint16_t adjLsaSize;
    uint16_t rchLsaSize;
} __attribute__((__packed__));

void RegisterDataPack::decode(const char* data, int dataLength) {
    const RegisterDataPackHeader* header = (const RegisterDataPackHeader*)(data);
    const char* ptr = data + sizeof(RegisterDataPackHeader);
    int adjLsaSize = ntoh<uint16_t>(header->adjLsaSize);
    int rchLsaSize = ntoh<uint16_t>(header->rchLsaSize);
    if (adjLsaSize != 0) {
        adjLsa = make_shared<LsaDataPack>();
        adjLsa->decode(ptr, adjLsaSize);
        ptr += adjLsaSize;

    } else {
        adjLsa = nullptr;
    }

    if (rchLsaSize != 0) {
        rchLsa = make_shared<LsaDataPack>();
        rchLsa->decode(ptr, rchLsaSize);
        ptr += rchLsaSize;
    } else {
        rchLsa = nullptr;
    }
}

pair<int, unique_ptr<char[]>> RegisterDataPack::encode() {
    RegisterDataPackHeader packetHeader;
    int adjLsaSize = 0, rchLsaSize = 0;
    if (adjLsa != nullptr) {
        adjLsaSize = adjLsa->getPacketSize();
    }

    if (rchLsa != nullptr) {
        rchLsaSize = rchLsa->getPacketSize();
    }
    packetHeader.adjLsaSize = hton<uint16_t>(adjLsaSize);
    packetHeader.rchLsaSize = hton<uint16_t>(rchLsaSize);

    int size = sizeof(RegisterDataPackHeader) + adjLsaSize + rchLsaSize;
    char* packet = new char[size];
    memcpy(packet, &packetHeader, sizeof(RegisterDataPackHeader));
    char* ptr = packet + sizeof(RegisterDataPackHeader);

    if (adjLsaSize != 0) {
        auto encodedAdj = adjLsa->encode();
        memcpy(ptr, encodedAdj.second.get(), adjLsaSize);
        ptr += adjLsaSize;
    }

    if (rchLsaSize != 0) {
        auto encodedRch = rchLsa->encode();
        memcpy(ptr, encodedRch.second.get(), rchLsaSize);
        ptr += rchLsaSize;
    }

    return {size, unique_ptr<char[]>(packet)};
}

nlohmann::json RegisterDataPack::marshal() const {
    json j;
    if (adjLsa != nullptr) {
        j["adjLsa"] = adjLsa->marshal();
    }
    if (rchLsa != nullptr) {
        j["rchLsa"] = rchLsa->marshal();
    }
    return j;
}