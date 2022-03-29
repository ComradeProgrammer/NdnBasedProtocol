#include "HelloInterestPack.h"
using namespace std;
using json = nlohmann::json;

// Internal class used to resolve or packup hello interest
struct HelloInterestHeader {
    RouterID _routerId;
    Ipv4Address _interfaceIP;
    Ipv4Address _networkMask;
    uint16_t _helloInterval;
    uint16_t _routerDeadInterval;
    uint32_t _neighborSize;
    char _databaseHash[16] = {0};
    char _signature[128] = {0};
    bool _hasPublicKey;
} __attribute__((__packed__));

HelloInterestPack::~HelloInterestPack() {
    if (publicKey == nullptr) {
        delete[] publicKey;
    }
}

pair<int, unique_ptr<char[]>> HelloInterestPack::encode() {
    // calculate the size
    HelloInterestHeader header;
    int size = sizeof(HelloInterestHeader) + 4 * neighbor.size();

    if (publicKey != nullptr) {
        size += 427;
        header._hasPublicKey = true;
    }
    char* buffer = new char[size];
    // populate the header
    header._routerId = hton(routerId);
    header._interfaceIP = interfaceIP;
    header._networkMask = networkMask;
    header._helloInterval = hton(helloInterval);
    header._routerDeadInterval = hton(routerDeadInterval);
    header._neighborSize = hton((uint32_t)(neighbor.size()));
    memcpy(header._signature, signature, 128);
    memcpy(header._databaseHash, databaseHash, 16);
    // insert the header;
    memcpy(buffer, &header, sizeof(HelloInterestHeader));
    // insert the neighbors
    char* ptr = buffer + sizeof(HelloInterestHeader);
    for (auto ip : neighbor) {
        memcpy(ptr, &ip, 4);
        ptr += 4;
    }
    // insert the public key
    if (publicKey != nullptr) {
        memcpy(ptr, publicKey, 427);
    }
    return {size, unique_ptr<char[]>(buffer)};
}

void HelloInterestPack::decode(const char* data, int dataLength) {
    // resolve the header
    const HelloInterestHeader* tmp = (const HelloInterestHeader*)(data);
    routerId = ntoh(tmp->_routerId);
    interfaceIP = tmp->_interfaceIP;
    networkMask = tmp->_networkMask;
    helloInterval = ntoh(tmp->_helloInterval);
    routerDeadInterval = ntoh(tmp->_routerDeadInterval);
    memcpy(databaseHash, tmp->_databaseHash, 16);
    memcpy(signature, tmp->_signature, 128);
    uint32_t neighbourSize = ntoh(tmp->_neighborSize);
    // resolve the neightbors;
    const Ipv4Address* neighbors = (const Ipv4Address*)(data + sizeof(HelloInterestHeader));
    for (int i = 0; i < neighbourSize; i++) {
        neighbor.push_back(neighbors[i]);
    }
    if (tmp->_hasPublicKey) {
        const char* ptr = (const char*)(data + sizeof(HelloInterestHeader) + neighbourSize * 4);
        publicKey = new char[427];
        memcpy(publicKey, ptr, 427);
    }
}

void HelloInterestPack::signSignature(std::string privateKey) {
    auto signatureGenerator = make_shared<Md5RsaSignatureFactory>();
    signatureGenerator->loadPrivateKey(privateKey);

    auto encodePair = encode();
    signatureGenerator->input(encodePair.second.get(), encodePair.first);

    auto signaturePair = signatureGenerator->generateSignature();
    memcpy(signature, signaturePair.first.get(), 128);
}

bool HelloInterestPack::verifySignature(std::string publicKey){
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

bool HelloInterestPack::verifySignature() {
    auto buffer = new unsigned char[128];
    memcpy(buffer, signature, 128);
    memset(signature, 0, 128);

    if(publicKey==nullptr){
        return false;
    }
    auto signatureVerifier = make_shared<Md5RsaSignatureFactory>();
    signatureVerifier->loadPublicKey(publicKey);

    auto encodePair = encode();
    signatureVerifier->input(encodePair.second.get(), encodePair.first);

    bool ok = signatureVerifier->verifySignature(buffer, 128);
    memcpy(signature, buffer, 128);
    return ok;
}

bool HelloInterestPack::verifyRouterID(){
    if(publicKey==nullptr){
        return false;
    }
    RouterID routerIDFronPublicKey=CityHash64(publicKey,427);
    return routerId==routerIDFronPublicKey;
}

json HelloInterestPack::marshal() const {
    json j;
    j["routerId"] = routerId;
    j["interfacIP"] = interfaceIP.toString();
    j["networkMask"] = networkMask.toString();
    j["helloInterval"] = helloInterval;
    j["routerDeadInterval"] = routerDeadInterval;
    j["databaseHash"] = hexString(databaseHash, 16);
    vector<string> res;
    for (auto i : neighbor) {
        res.push_back(i.toString());
    }
    j["neighbor"] = res;
    return j;
}
