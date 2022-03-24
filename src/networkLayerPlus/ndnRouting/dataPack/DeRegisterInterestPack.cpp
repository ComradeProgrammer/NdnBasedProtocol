#include "DeRegisterInterestPack.h"
using namespace std;
using nlohmann::json;

struct DeRegisterInterestInnerPack {
    RouterID root;
    char signatureOfPacketName[128] = {0};
} __attribute__((__packed__));

void DeRegisterInterestPack::decode(const char* data, int dataLength) {
    const DeRegisterInterestInnerPack* ptr = (const DeRegisterInterestInnerPack*)(data);
    root = ntoh(ptr->root);
    memcpy(signatureOfPacketName, ptr->signatureOfPacketName, 128);

}

pair<int, std::unique_ptr<char[]>> DeRegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = sizeof(DeRegisterInterestInnerPack);
    char* buffer = new char[size];
    DeRegisterInterestInnerPack* ptr = (DeRegisterInterestInnerPack*)(buffer);
    ptr->root = hton(root);
    memcpy(ptr->signatureOfPacketName, signatureOfPacketName, 128);
    return {size, unique_ptr<char[]>(buffer)};
}

json DeRegisterInterestPack::marshal() const {
    json j;
    j["root"] = root;
    return j;
}

void DeRegisterInterestPack::signatureGenerate(string packetName, string privateKey) {
    auto signatureGenerator = make_shared<Md5RsaSignatureFactory>();
    signatureGenerator->loadPrivateKey(privateKey);

    signatureGenerator->input(packetName.c_str(), packetName.size() + 1);
    auto signaturePair = signatureGenerator->generateSignature();
    memcpy(signatureOfPacketName, signaturePair.first.get(), 128);
}

bool DeRegisterInterestPack::validateSignature(std::string packetName, string publicKey) {
    auto signatureVerifier = make_shared<Md5RsaSignatureFactory>();
    signatureVerifier->loadPublicKey(publicKey);
    signatureVerifier->input(packetName.c_str(), packetName.size() + 1);
    bool ok = signatureVerifier->verifySignature((const unsigned char*)signatureOfPacketName, 128);
    return ok;
}