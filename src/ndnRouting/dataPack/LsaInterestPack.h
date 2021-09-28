#ifndef __LSAINTERESTPACK_H_
#define __LSAINTERESTPACK_H_
#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "PacketCommon.h"
class LsaInterestPack {
   public:
    uint32_t routerID;
    LinkStateType lsType;
    int32_t sequenceNum;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
};

#endif