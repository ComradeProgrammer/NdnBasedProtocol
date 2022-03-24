#ifndef __DEREGISTER_INTEREST_PACK_H_
#define __DEREGISTER_INTEREST_PACK_H_
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"
#include "util/signature/Md5RsaSignatureFactory.h"

class DeRegisterInterestPack : public Jsonfiable {
   public:
    RouterID root;
    char signatureOfPacketName[128] = {0};

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
    void signatureGenerate(std::string packetName, std::string privateKey);
    bool validateSignature(std::string packetName, std::string publicKey);
};

#endif