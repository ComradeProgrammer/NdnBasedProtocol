#ifndef __LSADATAPACK_H_
#define __LSADATAPACK_H_
#include<cstdint>
#include"NdnLink.h"
#include<vector>
#include"PacketCommon.h"
class LsaDataPack{
    public:
    LinkStateType lsType;
    uint32_t routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;
    std::vector<NdnLink>links;
    public:
    void decode(const char* data, int dataLength);
    std::pair<int,std::unique_ptr<char[]>>encode();
};
#endif