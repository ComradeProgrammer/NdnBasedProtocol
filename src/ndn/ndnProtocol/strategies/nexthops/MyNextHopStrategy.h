#ifndef __MYNEXTHOPSTRATEGY_H_
#define __MYNEXTHOPSTRATEGY_H_
#include "NextHopStrategyBase.h"
#include "ethernet/interface/NIC.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
class MyNextHopStrategy : public NextHopStrategyBase {
   public:
    /**
     * @brief Strategy: send this packet through all interfaces and upper layer
     * protocols, except the incoming interface.
     */
    virtual std::vector<std::pair<int, MacAddress>> operator()(int interfaceIndex, MacAddress sourceMac,
                                                               std::shared_ptr<NdnInterest> interest) override {
        std::vector<std::pair<int, MacAddress>> res;
        auto splits = split(interest->getName(), "/");
        if (splits.size() > 1 && splits[1] == "routing") {
            // special rule for routing
            if (splits.size() > 3 && splits[3] == "hello" && interfaceIndex != NDN_ROUTING) {
                res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
            } else if (splits.size() > 3 && splits[3] == "dd" && interfaceIndex != NDN_ROUTING) {
                res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
            } else if (splits.size() > 3 && splits[3] == "LSA") {
                if (splits[2] == "local") {
                    if (interfaceIndex != NDN_ROUTING) {
                        res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
                    }
                } else {
                    if (interfaceIndex != NDN_ROUTING) {
                        res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
                    }

                    auto allNic = NIC::getAllInterfaces();
                    for (int i = 0; i < allNic.size(); i++) {
                        if (allNic[i].getInterfaceID() == interfaceIndex) {
                            continue;
                        }
                        res.push_back({allNic[i].getInterfaceID(), MacAddress("ff:ff:ff:ff:ff:ff")});
                    }
                }
            } else if (splits.size() > 3 && splits[3] == "INFO") {
                if (interfaceIndex != NDN_ROUTING) {
                    res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
                }
                // to all
                auto allNic = NIC::getAllInterfaces();
                for (int i = 0; i < allNic.size(); i++) {
                    if (allNic[i].getInterfaceID() == interfaceIndex) {
                        continue;
                    }
                    res.push_back({allNic[i].getInterfaceID(), MacAddress("ff:ff:ff:ff:ff:ff")});
                }
            }
        } else {
            // just send to all
            auto allNic = NIC::getAllInterfaces();
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
        }

        return res;
    }
};
#endif