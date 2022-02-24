#ifndef __NDN_ROUTING_PROTOCOL_H_
#define __NDN_ROUTING_PROTOCOL_H_
#include <list>
#include <mutex>
#include <unordered_map>

#include "ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnRouting/controller/CronJobHandler.h"
#include "networkLayerPlus/ndnRouting/controller/DDController.h"
#include "networkLayerPlus/ndnRouting/controller/HelloController.h"
#include "networkLayerPlus/ndnRouting/controller/LsaController.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/model/lsaDatabase/LsaDatabase.h"
#include "physicalLayer/nic/NicManager.h"

class NdnRoutingProtocol : public NdnProtocolPlus, public std::enable_shared_from_this<NdnRoutingProtocol> {
   public:
    NdnRoutingProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol);
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

    RouterID getRouterID() { return routerID; }
    std::shared_ptr<CronJobHandler> getCrobJobHandler() { return cronJobHandler; }
    std::shared_ptr<LsaDatabase> getLsaDatabase() { return database; }

    std::shared_ptr<LsaDataPack>generateLsa();

    bool inBroadcastLsaPendingRequestList(LinkStateType lsaType, RouterID routerID, uint32_t sequenceNum);

    void lock() { mutexLock->lock(); }
    void unlock() { mutexLock->unlock(); }

    friend class Controller;
    friend class HelloController;
    friend class CronJobHandler;
    friend class DDController;
    friend class LsaController;

   private:
    std::shared_ptr<std::mutex> mutexLock;
    RouterID routerID;
    std::unordered_map<int, std::shared_ptr<NdnRoutingInterface>> interfaces;
    std::shared_ptr<LsaDatabase> database;
    std::list<LinkStateDigest> broadcastLsaPendingRequestList;

    std::shared_ptr<NdnProtocol> ndnProtocol;

    std::shared_ptr<CronJobHandler> cronJobHandler;
    std::shared_ptr<HelloController> helloController;
    std::shared_ptr<DDController> ddController;
    std::shared_ptr<LsaController> lsaController;
};
#endif