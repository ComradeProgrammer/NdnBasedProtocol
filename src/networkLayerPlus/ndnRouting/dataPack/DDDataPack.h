#ifndef __DDDATAPACK_H_
#define __DDDATAPACK_H_
#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "LinkStateDigest.h"
#include "util/printable/Jsonifiable.h"
#include "util/signature/Md5RsaSignatureFactory.h"

class DDDataPack : public Jsonfiable {
   public:
    RouterID neighbor;
    // uint16_t idx;//标识这是第几个分片,其实和名字里的那个是一样的
    uint16_t interfaceMTU;
    uint16_t numberOfDDPackets;
    std::vector<LinkStateDigest> ls;
    char signature[128] = {0};
   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    //调用此函数前，应当确保signature为全0 算出的签名是signature为全0时候的签名
    void signSignature(std::string privateKey);
    bool verifySignature(std::string publicKey);
    virtual nlohmann::json marshal() const override;
};
#endif