#ifndef __HELLOINTERESTPACK_H_
#define __HELLOINTERESTPACK_H_
#include <arpa/inet.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "PacketCommon.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "util/hash/cityhash.h"
#include "util/printable/Jsonifiable.h"
#include "util/signature/Md5RsaSignatureFactory.h"

class HelloInterestPack : public Jsonfiable {
   public:
    RouterID routerId;
    Ipv4Address interfaceIP;
    Ipv4Address networkMask;
    uint16_t helloInterval;
    uint16_t routerDeadInterval;
    std::vector<Ipv4Address> neighbor;
    unsigned char databaseHash[16];
    char signature[128] = {0};
    char* publicKey = nullptr;  // PUBLIC_KEY_LENGTH byte

   public:
    HelloInterestPack() = default;
    ~HelloInterestPack();
    HelloInterestPack(const HelloInterestPack& o) = delete;
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    //调用此函数前，应当确保signature为全0 算出的签名是signature为全0时候的签名
    void signSignature(std::string privateKey);
    bool verifySignature();
    bool verifySignature(std::string publicKey);
    //验证routerID是不是和公钥绑定的router ID
    bool verifyRouterID();
    bool verifyRouterID(std::string publicKey);
    virtual nlohmann::json marshal() const override;
};

#endif