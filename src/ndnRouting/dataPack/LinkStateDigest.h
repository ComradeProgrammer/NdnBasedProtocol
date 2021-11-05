 #ifndef __LINKSTATEDIGEST_H_
#define __LINKSTATEDIGEST_H_
#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>
#include"util/printable/Jsonifiable.h"
#include "PacketCommon.h"
class LinkStateDigest:public Jsonfiable{
   public:
    uint32_t routerID;
    LinkStateType linkStateType;
    int32_t sequenceNum;
    uint16_t lsAge;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    //smaller means older lsa
    bool operator<(const LinkStateDigest& o);
    virtual nlohmann::json marshal() override;


};

struct LinkStateDigestPacket {
    uint32_t routerID;
    uint16_t linkStateType;
    int32_t sequenceNum;
    uint16_t lsAge;
} __attribute__((__packed__));

#endif