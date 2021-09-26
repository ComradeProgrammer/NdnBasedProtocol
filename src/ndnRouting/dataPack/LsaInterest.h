#ifndef __LSAINTEREST_H_
#define __LSAINTEREST_H_
#include<cstdint>
#include"PacketCommon.h"
class LsaInterest{
    uint32_t routerID;
    LinkStateType lsType;
    int32_t sequenceNum;
};

#endif