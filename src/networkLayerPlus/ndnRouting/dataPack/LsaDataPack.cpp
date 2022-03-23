#include "LsaDataPack.h"
using namespace std;
using json = nlohmann::json;

struct LsaDataPackHeader {
    uint16_t lsType;
    RouterID routerID;
    int32_t seqNum;
    uint16_t lsAge;
    char publicKey[427]={0};
    char signature[128]={0};
    int16_t numberOfLinks;
} __attribute__((__packed__));

void LsaDataPack::decode(const char* data, int dataLength) {
    const LsaDataPackHeader* header = (const LsaDataPackHeader*)data;
    lsType = (LinkStateType)(ntoh(header->lsType));
    routerID = ntoh(header->routerID);
    seqNum = ntoh(header->seqNum);
    lsAge = ntoh(header->lsAge);
    memcpy(signature, header->signature, 128);
    memcpy(publicKey, header->publicKey, 427);
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
    memcpy(header.publicKey, publicKey, 427);
    memcpy(header.signature, signature, 128);

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

shared_ptr<NdnInterest> LsaDataPack::generateInfoInterest() {
    auto packet = make_shared<NdnInterest>();
    string infoType = getNameForLinkStateType(lsType);

    string name = "/routing/hop/INFO/" + infoType + "/" + to_string(routerID) + "/" + to_string(seqNum);

    packet->setName(name);
    packet->setNonce(rand());
    return packet;
}

bool LsaDataPack::operator<(const LsaDataPack& o) {
    if (lsType != o.lsType) {
        return lsType < o.lsType;
    }

    if (routerID != o.routerID) {
        return routerID < o.routerID;
    }

    if (seqNum != o.seqNum) {
        return seqNum < o.seqNum;
    }
    return lsAge > o.lsAge;
}

void LsaDataPack::signSignature(std::string privateKey) {
    auto signatureGenerator=make_shared<Md5RsaSignatureFactory>();
    signatureGenerator->loadPrivateKey(privateKey);

    auto encodePair=encode();
    signatureGenerator->input(encodePair.second.get(),encodePair.first);

    auto signaturePair=signatureGenerator->generateSignature();
    memcpy(signature,signaturePair.first.get(),128);
    
}
bool LsaDataPack::verifySignature() {
    auto buffer=new unsigned char[128];
    memcpy(buffer,signature,128);
    memset(signature,0,128);

    auto signatureVerifier=make_shared<Md5RsaSignatureFactory>();
    signatureVerifier->loadPublicKey(publicKey);

    auto encodePair=encode();
    signatureVerifier->input(encodePair.second.get(),encodePair.first);
    
    bool ok=signatureVerifier->verifySignature(buffer,128);
    memcpy(signature,buffer,128);
    return ok;
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
