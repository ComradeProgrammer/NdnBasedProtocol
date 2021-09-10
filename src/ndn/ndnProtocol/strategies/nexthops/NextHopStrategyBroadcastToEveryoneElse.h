#ifndef __NEXTHOPSTRATEGYBROADCASTTOEVERYONEELSE_H_
#define __NEXTHOPSTRATEGYBROADCASTTOEVERYONEELSE_H_
#include"NextHopStrategyBase.h"
#include"ethernet/interface/NIC.h"
class NextHopStrategyBroadcastToEveryoneElse: public  NextHopStrategyBase{
    public:
    /**
     * @brief Strategy: send this packet through all interfaces and upper layer protocols, except the incoming interface.
     */
    virtual std::vector<std::pair<int,MacAddress>>operator()(int interfaceIndex, MacAddress sourceMac,std::shared_ptr<NdnInterest> interest) override{
        std::vector<std::pair<int,MacAddress>>res;
        //just send to all
        auto allNic=NIC::getAllInterfaces();
        for(int i=0;i<allNic.size();i++){
            if(allNic[i].getInterfaceID()==interfaceIndex){
                continue;
            }
            res.push_back({allNic[i].getInterfaceID(),MacAddress("ff:ff:ff:ff:ff:ff")});
        }
        //TODO:add upper layer protocols to this strategy once it is set up
        return res;
    }
};
#endif