#ifndef __LSA_CONTROLLER_H_
#define __LSA_CONTROLLER_H_
#include "Controller.h"
#include "networkLayerPlus/ndnRouting/dataPack/LsaInterestPack.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
class LsaController: public Controller{
    public:
    LsaController(NdnRoutingProtocol* _protocol) : Controller(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) override;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) override;
};
#endif