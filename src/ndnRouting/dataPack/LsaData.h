#ifndef __LSADATA_H_
#define __LSADATA_H_
#include<cstdint>
#include<NdnLink.h>
#include<vector>
#include"PacketCommon.h"
class LsaData{
    LinkStateType lsType;
    uint32_t routerID;
    int32_t seqNum;
    uint16_t lsAge;
    int16_t numberOfLinks;
    std::vector<NdnLink>links;
};
#endif