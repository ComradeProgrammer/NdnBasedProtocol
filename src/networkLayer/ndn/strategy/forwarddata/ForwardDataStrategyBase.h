#ifndef __FORWARDDATASTRAGETYBASE_H_
#define __FORWARDDATASTRAGETYBASE_H_
#include <vector>

#include "linkLayer/MacAddress.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/component/pit/Pit.h"
class ForwardDataStrategyBase {
   public:
    virtual std::vector<std::pair<int, MacAddress>> operator()(int interfaceIndex, MacAddress sourceMac,
                                                               std::shared_ptr<NdnData> data,
                                                               std::shared_ptr<PitEntry> pitEntry) = 0;
};
#endif
