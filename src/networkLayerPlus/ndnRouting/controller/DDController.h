#ifndef __DD_INTEREST_CONTROLLER_H_
#define __DD_INTEREST_CONTROLLER_H_
#include "Controller.h"
#include "networkLayerPlus/ndnRouting/dataPack/HelloInterestPack.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
class DDController : public Controller {
   public:
    DDController(NdnRoutingProtocol* _protocol) : Controller(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) override;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) override;
};
#endif