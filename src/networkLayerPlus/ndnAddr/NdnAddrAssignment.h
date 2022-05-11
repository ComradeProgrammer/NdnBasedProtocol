#ifndef __NDN_ADDR_ASSIGNMENT_PROTOCOL_H_
#define __NDN_ADDR_ASSIGNMENT_PROTOCOL_H_
#include "ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceState.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateLeader.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateDown.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateNormal.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateWaiting.h"


class NdnAddrAssignmentProtocol : public NdnProtocolPlus {
   public:
    NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol);
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    void start();
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);

   private:
    std::shared_ptr<std::mutex> mutexLock;
    RouterID routerID;
    std::shared_ptr<NdnProtocol> ndnProtocol;

};
#endif