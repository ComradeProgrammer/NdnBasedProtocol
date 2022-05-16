#ifndef __MYNEXTHOPSTRATEGY_H_
#define __MYNEXTHOPSTRATEGY_H_
#include "NextHopStrategyBase.h"
#include "ioc.h"
#include "linkLayer/MacAddress.h"
#include "networkLayer/ndn/component/pit/Pit.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "physicalLayer/nic/NicManager.h"
class MyNextHopStrategy : public NextHopStrategyBase {
   public:
    /**
     * @brief Strategy: send this packet through all interfaces and upper layer
     * protocols, except the incoming interface.
     */
    virtual std::vector<std::pair<int, MacAddress>> operator()(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) override {
        std::vector<std::pair<int, MacAddress>> res;
        auto splits = split(interest->getName(), "/");
        if (splits.size() > 3 && splits[1] == "rt" && interfaceIndex != NDN_ROUTING && (splits[2] == "local" || splits[2] == "hop")) {
            res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
        } else if (splits.size() > 3 && splits[1] == "addr" && interfaceIndex != NDN_ADDRASSIGNMENT && (splits[2] == "local" || splits[2] == "hop")) {
            res.push_back({NDN_ADDRASSIGNMENT, MacAddress("00:00:00:00:00:00")});
        } else {
            auto allNic = IOC->getNicManager()->getAllNicsInMap();
            for (auto pair : allNic) {
                if (pair.second->getInterfaceID() == interfaceIndex) {
                    continue;
                }
                res.push_back({pair.second->getInterfaceID(), MacAddress("ff:ff:ff:ff:ff:ff")});
            }
        }
        return res;
    }
};
#endif