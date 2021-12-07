#include "LsaDataPack.h"
using namespace std;
using json = nlohmann::json;

struct LsaDataPackHeader {
    uint16_t lsType;
    uint32_t routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;
} __attribute__((__packed__));

void LsaDataPack::decode(const char* data, int dataLength) {
    const LsaDataPackHeader* header = (const LsaDataPackHeader*)data;
    lsType = (LinkStateType)(ntohs(header->lsType));
    routerID = ntohl(header->routerID);
    seqNum = ntohl(header->seqNum);
    lsAge = ntohs(header->lsAge);
    numberOfLinks = ntohl(header->numberOfLinks);
    const NdnLinkPacket* ptr = (const NdnLinkPacket*)(data + sizeof(LsaDataPackHeader));
    while ((const char*)ptr < data + dataLength) {
        NdnLink tmp;
        tmp.decode((const char*)ptr, sizeof(NdnLinkPacket));
        links.push_back(tmp);
        ptr++;
    }
}

pair<int, std::unique_ptr<char[]>> LsaDataPack::encode() {
    LsaDataPackHeader header;
    header.lsType = htons((uint16_t)lsType);
    header.routerID = htonl(routerID);
    header.seqNum = htonl(seqNum);
    header.lsAge = htons(lsAge);
    header.numberOfLinks = htons(numberOfLinks);

    int size = sizeof(LsaDataPackHeader) + links.size() * sizeof(NdnLinkPacket);
    char* buffer = new char[size];
    char* ptr = buffer + sizeof(LsaDataPackHeader);
    memcpy(buffer, &header, sizeof(LsaDataPackHeader));
    for (int i = 0; i < links.size(); i++) {
        auto tmp = links[i].encode();
        memcpy(ptr, tmp.second.get(), tmp.first);
        ptr += tmp.first;
    }
    return {size, unique_ptr<char[]>(buffer)};
}

LinkStateDigest LsaDataPack::generateLSDigest() const {
    LinkStateDigest digest;
    digest.routerID = routerID;
    digest.linkStateType = lsType;
    digest.sequenceNum = seqNum;
    digest.lsAge = lsAge;
    return digest;
}
json LsaDataPack::marshal() const {
    json j;
    j["lsType"] = getNameForLinkStateType(lsType);
    j["routerID"] = routerID;
    j["seqNum"] = seqNum;
    j["lsAge"] = lsAge;
    j["numberOfLinks"] = numberOfLinks;
    vector<json> tmp;
    for (auto i : links) {
        tmp.push_back(i.marshal());
    }
    j["links"] = tmp;
    return j;
}
