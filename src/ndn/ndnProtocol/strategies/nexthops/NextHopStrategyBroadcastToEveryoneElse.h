#ifndef __NEXTHOPSTRATEGYBROADCASTTOEVERYONEELSE_H_
#define __NEXTHOPSTRATEGYBROADCASTTOEVERYONEELSE_H_
#include "NextHopStrategyBase.h"
#include "ethernet/interface/NICManager.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
class NextHopStrategyBroadcastToEveryoneElse : public NextHopStrategyBase {
   public:
    /**
     * @brief Strategy: send this packet through all interfaces and upper layer
     * protocols, except the incoming interface.
     */
    virtual std::vector<std::pair<int, MacAddress>> operator()(int interfaceIndex, MacAddress sourceMac,
                                                               std::shared_ptr<NdnInterest> interest) override {
        std::vector<std::pair<int, MacAddress>> res;
        // just send to all
        auto allNic = NICManager::getNICManager()->getAllInterfaces();
        for (int i = 0; i < allNic.size(); i++) {
            if (allNic[i].getInterfaceID() == interfaceIndex) {
                continue;
            }
            res.push_back({allNic[i].getInterfaceID(), MacAddress("ff:ff:ff:ff:ff:ff")});
        }
        // add upper layer protocols to this strategy
        for (auto pair : NdnProtocol::getRegisteredUpperLayerProtocol()) {
            if (pair.first == interfaceIndex) {
                continue;
            }
            res.push_back({pair.first, MacAddress("00:00:00:00:00:00")});
        }
        return res;
    }
};
#endif