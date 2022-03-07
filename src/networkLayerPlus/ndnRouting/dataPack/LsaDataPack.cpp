#include "LsaDataPack.h"
using namespace std;
using json = nlohmann::json;

struct LsaDataPackHeader {
    uint16_t lsType;
    RouterID routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;
} __attribute__((__packed__));

void LsaDataPack::decode(const char* data, int dataLength) {
    const LsaDataPackHeader* header = (const LsaDataPackHeader*)data;
    lsType = (LinkStateType)(ntoh(header->lsType));
    routerID = ntoh(header->routerID);
    seqNum = ntoh(header->seqNum);
    lsAge = ntoh(header->lsAge);
    numberOfLinks = ntoh(header->numberOfLinks);
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
    header.lsType = hton((uint16_t)lsType);
    header.routerID = hton(routerID);
    header.seqNum = hton(seqNum);
    header.lsAge = hton(lsAge);
    header.numberOfLinks = hton(numberOfLinks);

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
int LsaDataPack::getPacketSize() const { return sizeof(LsaDataPackHeader) + links.size() * sizeof(NdnLinkPacket); }

shared_ptr<NdnInterest> LsaDataPack::generateInfoInterest(){
    auto packet = make_shared<NdnInterest>();
    string infoType=getNameForLinkStateType(lsType);

    string name ="/routing/hop/INFO/"+infoType+"/"+to_string(routerID) + "/"+to_string(seqNum);

    packet->setName(name);
    packet->setNonce(rand());
    return packet;
    
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
