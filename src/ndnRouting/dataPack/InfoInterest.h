#ifndef __INFOINTEREST_H_
#define __INFOINTEREST_H_  
#include<cstdint>
#include<vector>
#include"LinkStateDigest.h"
enum InfoType
{
    DOWN =0,
    UP =1,
    REFRESH =2
};
class InfoInterest{
    InfoType infoType;
    uint32_t src;
    std::vector<LinkStateDigest>ls;
    std::vector<uint32_t>neighbors;
};
#endif