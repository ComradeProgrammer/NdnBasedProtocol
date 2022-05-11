#ifndef __LSADATAPACK_H_
#define __LSADATAPACK_H_
#include <cstdint>
#include <vector>

#include "LinkStateDigest.h"
#include "NdnLink.h"
#include "PacketCommon.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "util/hash/cityhash.h"
#include "util/printable/Jsonifiable.h"

class LsaDataPack : public Jsonfiable {
   public:
    LinkStateType lsType;
    RouterID routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;

    std::vector<NdnLink> links;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
    bool operator<(const LsaDataPack& o);

    LinkStateDigest generateLSDigest() const;
    int getPacketSize() const;
    // generate an info interest. No specified target specified.
    std::shared_ptr<NdnInterest> generateInfoInterest();
};
#endif