#ifndef __REGISTER_INTEREST_PACK_H_
#define __REGISTER_INTEREST_PACK_H_
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"
#include "util/signature/Md5RsaSignatureFactory.h"

class RegisterInterestPack : public Jsonfiable {
   public:
    /**
     * @brief when A send a RegisterInterestPack to B with roots=1, that means A register B as parent in minimum-hop trees whose root is 1
     */
    RouterID root;
    // currently we only exchange this ADJ message.
    int32_t adjSequenceNum;
    int32_t rchSequenceNum;

    // this is the signature of the interest name
    char signatureOfPacketName[128] = {0};

   public:
    void signatureGenerate(std::string packetName, std::string privateKey);
    bool validateSignature(std::string packetName, std::string publicKey);

    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};
#endif