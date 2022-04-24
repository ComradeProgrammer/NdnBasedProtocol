#ifndef __DEREGISTER_CONTROLLER_H_
#define __DEREGISTER_CONTROLLER_H_
#include "Controller.h"
#include "networkLayerPlus/ndnRouting/dataPack/DeRegisterInterestPack.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
class DeRegisterController : public Controller {
   public:
    DeRegisterController(NdnRoutingProtocol* _protocol) : Controller(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) override;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) override;
};
#endif
