#ifndef __NDN_ADDR_ASSIGNMENT_PROTOCOL_H_
#define __NDN_ADDR_ASSIGNMENT_PROTOCOL_H_
#include "ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnAddr/controller/AddrCronjobController.h"
#include "networkLayerPlus/ndnAddr/controller/AddrHelloController.h"
#include "networkLayerPlus/ndnAddr/model/interface/NdnAddrInterface.h"
class NdnAddrAssignmentProtocol : public NdnProtocolPlus {
   public:
    NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol);
    RouterID getRouterID() { return routerID; }
    void setRouterID(RouterID rid) { routerID = rid; }

    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    void start();
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    void lock() { mutexLock->lock(); }
    void unlock() { mutexLock->unlock(); }

    std::shared_ptr<AddrCronjobController> getCronjobHandler() { return cronjobController; }

   private:
    std::shared_ptr<std::mutex> mutexLock;
    RouterID routerID;
    std::shared_ptr<NdnProtocol> ndnProtocol;
    std::unordered_map<int, std::shared_ptr<NdnAddrInterface>> interfaces;
    std::shared_ptr<AddrCronjobController> cronjobController;
    std::shared_ptr<AddrHelloController> helloController;

    friend class AddrCronjobController;
    friend class AddrHelloController;
};
#endif