#ifndef __NEXTHOPSTRATEGYBASE_H_
#define __NEXTHOPSTRATEGYBASE_H_
#include<vector>
#include "ethernet/ethernetPacket/MacAddress.h"
#include "ndn/ndnPacket/NdnInterest.h"
class NextHopStrategyBase{
    public:
    virtual std::vector<int>operator()(int interfaceIndex, MacAddress sourceMac,
                                     std::shared_ptr<NdnInterest> interest)=0;
};
#endif