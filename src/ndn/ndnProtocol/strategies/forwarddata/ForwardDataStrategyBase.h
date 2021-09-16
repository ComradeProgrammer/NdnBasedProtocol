#ifndef __FORWARDDATASTRAGETYBASE_H_
#define __FORWARDDATASTRAGETYBASE_H_
#include<vector>
#include "ethernet/ethernetPacket/MacAddress.h"
#include "ndn/ndnPacket/NdnData.h"
#include "ndn/ndnProtocol/components/pit/Pit.h"
class ForwardDataStrategyBase{
    public:
    virtual std::vector<std::pair<int,MacAddress>>operator()(int interfaceIndex, MacAddress sourceMac,std::shared_ptr<NdnData> data,std::shared_ptr<PitEntry> pitEntry)=0;
};
#endif
