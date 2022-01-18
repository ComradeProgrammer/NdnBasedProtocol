#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_
#include"networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
class Controller{
    public:
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet)=0;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet)=0;
};
#endif