#include "InfoInterestPack.h"
using namespace std;
using json = nlohmann::json;

struct InfoInterestPackHeader {
    uint16_t infoType;
    uint16_t ptrToNeighbors;
    RouterID src;
} __attribute__((__packed__));

void InfoInterestPack::decode(const char* data, int dataLength) {
    const InfoInterestPackHeader* header = (const InfoInterestPackHeader*)data;
    infoType = (InfoType)(ntoh(header->infoType));
    src = ntoh(header->src);
    int ptrToNeighbors = ntoh(header->ptrToNeighbors);

    // resolve the link state digest array
    LinkStateDigestPacket* ptr1 = (LinkStateDigestPacket*)(data + sizeof(InfoInterestPackHeader));
    int lsSize = (ptrToNeighbors - sizeof(InfoInterestPackHeader)) / (sizeof(LinkStateDigestPacket));
    for (int i = 0; i < lsSize; i++) {
        LinkStateDigest digest;
        digest.decode((const char*)(ptr1 + i), sizeof(LinkStateDigestPacket));
        ls.push_back(digest);
    }
    // decode the neighbors array
    RouterID* ptr2 = (RouterID*)(data + ptrToNeighbors);
    int neighborSize = (dataLength - ptrToNeighbors) / sizeof(RouterID);
    for (int i = 0; i < neighborSize; i++) {
        neighbors.push_back(ntoh(ptr2[i]));
    }
}

pair<int, unique_ptr<char[]>> InfoInterestPack::encode() {
    // calculate the packet size;
    int size = sizeof(InfoInterestPackHeader) + sizeof(LinkStateDigestPacket) * ls.size() + sizeof(RouterID) * neighbors.size();
    char* buffer = new char[size];
    InfoInterestPackHeader* header = (InfoInterestPackHeader*)buffer;
    // encode the header part
    header->infoType = hton((uint16_t)infoType);
    header->src = hton(src);
    header->ptrToNeighbors = htons(sizeof(InfoInterestPackHeader) + sizeof(LinkStateDigestPacket) * ls.size());

    // encode the link state digest array
    char* ptr = buffer + sizeof(InfoInterestPackHeader);
    for (int i = 0; i < ls.size(); i++) {
        auto tmp = ls[i].encode();
        memcpy(ptr, tmp.second.get(), tmp.first);
        ptr += tmp.first;
    }

    // encode the neighbors array
    RouterID* ptr2 = (RouterID*)ptr;
    for (int i = 0; i < neighbors.size(); i++) {
        *ptr2 = hton(neighbors[i]);
        ptr2++;
    }
    return {size, unique_ptr<char[]>(buffer)};
}

json InfoInterestPack::marshal() const {
    json j;
    switch (infoType) {
        case InfoType::INFO_DOWN:
            j["infoType"] = "DOWN";
            break;
        case InfoType::INFO_UP:
            j["infoType"] = "UP";
            break;
        case InfoType::INFO_REFRESH:
            j["infoType"] = "refresh";
    }
    j["src"] = src;
    vector<json> lsString;
    for (auto i : ls) {
        lsString.push_back(i.marshal());
    }
    j["ls"] = lsString;
    j["neighbors"] = neighbors;

    return j;
}