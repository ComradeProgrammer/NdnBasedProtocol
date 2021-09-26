#ifndef __LINKSTATEDIGEST_H_
#define __LINKSTATEDIGEST_H_
#include<cstdint>
#include<vector>
#include<unordered_map>
#include<memory>
#include<cstring>
#include <arpa/inet.h>
#include"PacketCommon.h"
class LinkStateDigest{
    public:
    uint32_t routerID;
    LinkStateType linkStateType;
    int32_t sequenceNum;
    uint16_t lsAge;
    public:
    void decode(const char* data, int dataLength);
    std::pair<int,std::unique_ptr<char[]>>encode();
};


struct LinkStateDigestPacket{
    uint32_t routerID;
    uint16_t linkStateType;
    int32_t sequenceNum;
    uint16_t lsAge;
}__attribute__((__packed__));


#endif