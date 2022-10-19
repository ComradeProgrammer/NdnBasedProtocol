#ifndef __ROUTING_CHAIN_CONTROLLER_H_
#define __ROUTING_CHAIN_CONTROLLER_H_
#include "Controller.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "util/audit/AuditRecorderInterface.h"
class RoutingChainController : public Controller {
    RoutingChainController(NdnRoutingProtocol* _protocol) : Controller(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) override;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) override;
};
#endif