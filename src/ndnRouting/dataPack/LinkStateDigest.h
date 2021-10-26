 #ifndef __LINKSTATEDIGEST_H_
#define __LINKSTATEDIGEST_H_
#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "PacketCommon.h"
class LinkStateDigest {
   public:
    uint32_t routerID;
    LinkStateType linkStateType;
    int32_t sequenceNum;
    uint16_t lsAge;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    bool operator<(const LinkStateDigest& o);
};

struct LinkStateDigestPacket {
    uint32_t routerID;
    uint16_t linkStateType;
    int32_t sequenceNum;
    uint16_t lsAge;
} __attribute__((__packed__));

#endif