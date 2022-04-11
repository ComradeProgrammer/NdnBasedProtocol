#include "RegisterInterestPack.h"

using namespace std;
using nlohmann::json;

struct RegisterInterestInnerPack {
    RouterID root;
    int32_t rchSequencesNum;
    int32_t adjSequencesNum;
    char signatureOfPacketName[128] = {0};
} __attribute__((__packed__));

void RegisterInterestPack::decode(const char* data, int dataLength) {
    const RegisterInterestInnerPack* ptr = (const RegisterInterestInnerPack*)(data);
    root = ntoh(ptr->root);
    rchSequenceNum = ntoh(ptr->rchSequencesNum);
    adjSequenceNum = ntoh(ptr->adjSequencesNum);
    memcpy(signatureOfPacketName, ptr->signatureOfPacketName, 128);
}

pair<int, std::unique_ptr<char[]>> RegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = sizeof(RegisterInterestInnerPack);
    char* buffer = new char[size];
    RegisterInterestInnerPack* ptr = (RegisterInterestInnerPack*)(buffer);
    ptr->root = hton(root);
    ptr->rchSequencesNum = hton(rchSequenceNum);
    ptr->adjSequencesNum = hton(adjSequenceNum);
    memcpy(ptr->signatureOfPacketName, signatureOfPacketName, 128);
    return {size, unique_ptr<char[]>(buffer)};
}

void RegisterInterestPack::signatureGenerate(string packetName, string privateKey) {
    shared_ptr<SignatureAbstractFactory> signatureGenerator = make_shared<Md5RsaSignatureFactory>();
    signatureGenerator->loadPrivateKey(privateKey);

    signatureGenerator->input(packetName.c_str(), packetName.size() + 1);
    auto signaturePair = signatureGenerator->generateSignature();
    memcpy(signatureOfPacketName, signaturePair.first.get(), 128);
}

bool RegisterInterestPack::validateSignature(std::string packetName, string publicKey) {
    shared_ptr<SignatureAbstractFactory> signatureVerifier = make_shared<Md5RsaSignatureFactory>();
    signatureVerifier->loadPublicKey(publicKey);
    signatureVerifier->input(packetName.c_str(), packetName.size() + 1);
    bool ok = signatureVerifier->verifySignature((const unsigned char*)signatureOfPacketName, 128);
    return ok;
}

json RegisterInterestPack::marshal() const {
    json j;
    j["root"] = root;
    j["adjSequenceNumber"] = adjSequenceNum;
    j["rchSequenceNumber"] = rchSequenceNum;
    return j;
}