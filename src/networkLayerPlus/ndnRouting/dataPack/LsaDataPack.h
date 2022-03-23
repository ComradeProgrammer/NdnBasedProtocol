#ifndef __LSADATAPACK_H_
#define __LSADATAPACK_H_
#include <cstdint>
#include <vector>

#include "LinkStateDigest.h"
#include "NdnLink.h"
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "util/signature/Md5RsaSignatureFactory.h"
class LsaDataPack : public Jsonfiable {
   public:
    LinkStateType lsType;
    RouterID routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;
    //1024bit的公钥字符串长度是427字符
    char publicKey[427]={0};
    char signature[128]={0};
    std::vector<NdnLink> links;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
    bool operator<(const LsaDataPack& o);

    //调用此函数前，应当确保signature为全0 算出的签名是signature为全0时候的签名
    void signSignature(std::string privateKey);
    bool verifySignature();

    LinkStateDigest generateLSDigest() const;
    int getPacketSize() const;
    //generate an info interest. No specified target specified.
    std::shared_ptr<NdnInterest>generateInfoInterest();
};
#endif