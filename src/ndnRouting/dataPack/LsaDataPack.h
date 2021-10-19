#ifndef __LSADATAPACK_H_
#define __LSADATAPACK_H_
#include <cstdint>
#include <vector>

#include "NdnLink.h"
#include "PacketCommon.h"
#include"LinkStateDigest.h"
class LsaDataPack {
   public:
    LinkStateType lsType;
    uint32_t routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;
    std::vector<NdnLink> links;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();

    LinkStateDigest generateLSDigest()const;
};
#endif