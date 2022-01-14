#ifndef __NDNLINK_H_
#define __NDNLINK_H_
#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "PacketCommon.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"

enum LinkType { POINTTOPOINT_LINK = 1, TRANSIT_LINK = 2, STUB_LINK = 3 };

class NdnLink : public Jsonfiable {
   public:
    LinkType linkType;
    RouterID linkID;
    uint32_t linkData;
    uint32_t linkDataMask;
    uint32_t linkCost;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

struct NdnLinkPacket {
    uint16_t linkType;
    RouterID linkID;
    uint32_t linkData;
    uint32_t linkDataMask;
    uint32_t linkCost;
} __attribute__((__packed__));
#endif