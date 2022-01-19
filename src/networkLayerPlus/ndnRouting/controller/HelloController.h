#ifndef __HELLO_CONTROLLER_H_
#define __HELLO_CONTROLLER_H_
#include"Controller.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/dataPack/HelloInterestPack.h"
class HelloController: public Controller{
    public:
    HelloController(NdnRoutingProtocol* _protocol) : Controller(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet)override;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet)override;
};
#endif