#ifndef __NDN_ROUTING_PROTOCOL_H_
#define __NDN_ROUTING_PROTOCOL_H_
#include <mutex>
#include <unordered_map>
#include"ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "physicalLayer/nic/NicManager.h"
#include "networkLayerPlus/ndnRouting/controller/CronJobHandler.h"
#include "networkLayerPlus/ndnRouting/controller/HelloController.h"


class NdnRoutingProtocol : public NdnProtocolPlus {
   public:
    NdnRoutingProtocol(RouterID _routerID,std::shared_ptr<NdnProtocol> _ndnProtocol) ;
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) override;

    /**
     * @brief initialize the protocol
     */
    void start();

    /**
     * @brief lock should have been released before call because this function
     * may attain the lock of NdnProtocol object
     */
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);

    RouterID getRouterID(){return routerID;}
    std::shared_ptr<CronJobHandler>getCrobJobHandler(){return cronJobHandler;}

    friend class Controller;
    friend class CronJobHandler;

   private:
    std::shared_ptr<std::mutex> lock;
    RouterID routerID;
    std::unordered_map<int, std::shared_ptr<NdnRoutingInterface>> interfaces;
    std::shared_ptr<NdnProtocol> ndnProtocol;

    std::shared_ptr<CronJobHandler>cronJobHandler;
    std::shared_ptr<HelloController>helloController;
    
};
#endif