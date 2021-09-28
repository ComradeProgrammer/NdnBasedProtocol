#ifndef __FORWARDDATASTRATEGYDEFAULT_H_
#define __FORWARDDATASTRATEGYDEFAULT_H_
#include "ForwardDataStrategyBase.h"
#include "ethernet/interface/NIC.h"
// satisfy all pending interest.
class ForwardDataStrategyDefault : public ForwardDataStrategyBase {
   public:
    virtual std::vector<std::pair<int, MacAddress>> operator()(
        int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data,
        std::shared_ptr<PitEntry> pitEntry) override {
        // just satisfy every pending interest
        std::vector<std::pair<int, MacAddress>> res;
        auto nicMap = NIC::getNICMap();
        auto faces = pitEntry->getAllPendingInterfaces();
        for (auto i : faces) {
            if (i == interfaceIndex) {
                continue;
            }
            // handle possible non-nic protocol faces.
            if (i < 0) {
                res.push_back({i, MacAddress("00:00:00:00:00:00")});
            } else {
                res.push_back({i, nicMap[i].getMacAddress()});
            }
        }
        return res;
    }
};
#endif