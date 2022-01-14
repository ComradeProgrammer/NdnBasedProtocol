#ifndef __MYFORWARDSTRATEGY_H_
#define __MYFORWARDSTRATEGY_H_
#include "ForwardDataStrategyBase.h"
#include "physicalLayer/nic/NicManager.h"
#include"ioc.h"
// satisfy all pending interest.
class MyForwardStrategy : public ForwardDataStrategyBase {
   public:
    virtual std::vector<std::pair<int, MacAddress>> operator()(int interfaceIndex, MacAddress sourceMac,
                                                               std::shared_ptr<NdnData> data,
                                                               std::shared_ptr<PitEntry> pitEntry) override {
        // just satisfy every pending interest
        std::vector<std::pair<int, MacAddress>> res;
        auto nicMap = IOC->getNicManager()->getAllNicsInMap();
        if (pitEntry != nullptr) {
            auto faces = pitEntry->getAllPendingInterfaces();
            for (auto i : faces) {
                if (i == interfaceIndex) {
                    continue;
                }
                // handle possible non-nic protocol faces.
                if (i > 0) {
                    res.push_back({i,  MacAddress("ff:ff:ff:ff:ff:ff")});
                }
            }
        }

        // special rule for ndn routing
        auto splits = split(data->getName(), "/");
        if (splits.size() > 1 && splits[1] == "routing"&&interfaceIndex!=NDN_ROUTING) {
            res.push_back({NDN_ROUTING, MacAddress("00:00:00:00:00:00")});
        }else if (splits.size() > 1 && splits[1] == "addr"&&interfaceIndex!=NDN_ADDRASSIGNMENT){
            res.push_back({NDN_ADDRASSIGNMENT, MacAddress("00:00:00:00:00:00")});
        }
        return res;
    }
};
#endif