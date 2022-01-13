#ifndef __NEXTHOPSTRATEGYBASE_H_
#define __NEXTHOPSTRATEGYBASE_H_
#include <vector>

#include "linkLayer/MacAddress.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/component/pit/Pit.h"
class NextHopStrategyBase {
   public:
    /**
     * @brief implement a strategy for choosing the interfaces(or upper layer
     * protocols )which this incoming packet should be sent to.
     * @return std::vector<int> the array of indices of the interafaces.
     */
    virtual std::vector<std::pair<int, MacAddress>> operator()(int interfaceIndex, MacAddress sourceMac,
                                                               std::shared_ptr<NdnInterest> interest) = 0;
};
#endif