#include "DDDataPack.h"
using namespace std;
using json = nlohmann::json;
struct DDDataPacketHeader {
    RouterID _neightbor;
    // uint16_t _idx;
    uint16_t _interfaceMTU;
    uint16_t _numberOfDDPackets;
    char signature[128] = {0};

} __attribute__((__packed__));

pair<int, unique_ptr<char[]>> DDDataPack::encode() {
    DDDataPacketHeader header;
    header._neightbor = hton(neighbor);
    // header._idx=
    header._interfaceMTU = hton(interfaceMTU);
    header._numberOfDDPackets = hton(numberOfDDPackets);
    memcpy(header.signature,signature,128);

    int size = sizeof(DDDataPacketHeader) + ls.size() * sizeof(LinkStateDigestPacket);
    char* buffer = new char[size];
    memcpy(buffer, &header, sizeof(header));

    char* ptr = buffer + sizeof(header);
    for (size_t i = 0; i < ls.size(); i++) {
        auto tmp = ls[i].encode();
        memcpy(ptr, tmp.second.get(), tmp.first);
        ptr += tmp.first;
    }
    return {size, unique_ptr<char[]>(buffer)};
}

void DDDataPack::decode(const char* data, int dataLength) {
    const DDDataPacketHeader* ptr = (const DDDataPacketHeader*)data;
    neighbor = ntoh(ptr->_neightbor);
    interfaceMTU = ntoh(ptr->_interfaceMTU);
    numberOfDDPackets = ntoh(ptr->_numberOfDDPackets);
    memcpy(signature,ptr->signature,128);

    const char* ptr2 = data + sizeof(DDDataPacketHeader);
    while (ptr2 < data + dataLength) {
        LinkStateDigest tmp;
        tmp.decode(ptr2, sizeof(LinkStateDigestPacket));
        ls.push_back(tmp);
        ptr2 += sizeof(LinkStateDigestPacket);
    }
}


void DDDataPack::signSignature(std::string privateKey) {
    auto signatureGenerator = make_shared<Md5RsaSignatureFactory>();
    signatureGenerator->loadPrivateKey(privateKey);

    auto encodePair = encode();
    signatureGenerator->input(encodePair.second.get(), encodePair.first);

    auto signaturePair = signatureGenerator->generateSignature();
    memcpy(signature, signaturePair.first.get(), 128);
}
bool DDDataPack::verifySignature(std::string publicKey){
    auto buffer = new unsigned char[128];
    memcpy(buffer, signature, 128);
    memset(signature, 0, 128);
    auto signatureVerifier = make_shared<Md5RsaSignatureFactory>();
    signatureVerifier->loadPublicKey(publicKey);

    auto encodePair = encode();
    signatureVerifier->input(encodePair.second.get(), encodePair.first);

    bool ok = signatureVerifier->verifySignature(buffer, 128);
    memcpy(signature, buffer, 128);
    return ok;
}


json DDDataPack::marshal() const {
    json j;
    j["neighbor"] = neighbor;
    j["interfaceMTU"] = interfaceMTU;
    j["numberOfDDPackets"] = numberOfDDPackets;
    vector<json> lsString;
    for (auto i : ls) {
        lsString.push_back(i.marshal());
    }
    j["ls"] = lsString;
    return j;
}