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
#include "networkLayerPlus/ndnRouting/controller/RegisterController.h"
#include "networkLayerPlus/ndnRouting/controller/DeRegisterController.h"
#include "networkLayerPlus/ndnRouting/controller/InfoController.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/dataPack/RegisterInterestPack.h"
#include "networkLayerPlus/ndnRouting/dataPack/DeRegisterInterestPack.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/model/lsaDatabase/LsaDatabase.h"
#include "physicalLayer/nic/NicManager.h"
#include "networkLayerPlus/ndnRouting/model/tree/MinimumHopTree.h"

class NdnRoutingProtocol : public NdnProtocolPlus, public std::enable_shared_from_this<NdnRoutingProtocol> {
   public:
    NdnRoutingProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol);
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) override;

    /**
     * @brief initialize the protocol
     */
    void start();


    RouterID getRouterID() { return routerID; }
    std::shared_ptr<LsaDatabase> getLsaDatabase() { return database; }
    /**
     * @brief get Neighbor object by Router ID
     * @return std::shared_ptr<NdnRoutingNeighbor>, nullptr if not found
     */
    std::shared_ptr<NdnRoutingNeighbor> getNeighborByRouterID(RouterID id);

    std::shared_ptr<CronJobHandler> getCrobJobHandler() { return cronJobHandler; }
    /**
     * @brief generate new lsa and replace the old one if it exists
     *
     * @return std::shared_ptr<LsaDataPack> new lsa
     */
    std::shared_ptr<LsaDataPack> generateLsa();

    bool inBroadcastLsaPendingRequestList(LinkStateType lsaType, RouterID routerID, uint32_t sequenceNum);
    void removeFromBroadcastLsaPendingRequestList(LinkStateType lsaType, RouterID routerID, uint32_t sequenceNum);
    /**
     * @brief recalculate all parents for all roots, and send register packets and deregister packets;
     */
    void registerParents();

    long sendRegisterPacket(RouterID root, RouterID parent);
    long sendDeregisterPacket(RouterID root, RouterID parent);
    void sendInfoToChildren(std::shared_ptr<LsaDataPack>lsa);
    void sendInfoToAll(std::shared_ptr<LsaDataPack>lsa,RouterID exemptedNeighbor);

    /**
     * @brief lock should have been released before call because this function
     * may attain the lock of NdnProtocol object. 源MAC地址填什么都可以。因为不是发给上层协议的包，源MAC都会被根据发出的接口被替换掉
     */
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    void lock() { mutexLock->lock(); }
    void unlock() { mutexLock->unlock(); }

    std::string databaseContent(){
        lock();
        return database->toString();
        unlock();
    }

    friend class Controller;
    friend class HelloController;
    friend class CronJobHandler;
    friend class DDController;
    friend class LsaController;
    friend class RegisterController;
    friend class InfoController;
    friend class DeRegisterController;

   private:
    std::shared_ptr<std::mutex> mutexLock;
    RouterID routerID;
    std::unordered_map<int, std::shared_ptr<NdnRoutingInterface>> interfaces;
    std::shared_ptr<LsaDatabase> database;
    std::list<LinkStateDigest> broadcastLsaPendingRequestList;
    std::shared_ptr<MinimumHopTree>minimumHopTree;
    
    std::shared_ptr<CronJobHandler> cronJobHandler;
    std::shared_ptr<HelloController> helloController;
    std::shared_ptr<DDController> ddController;
    std::shared_ptr<LsaController> lsaController;
    std::shared_ptr<RegisterController> registerController;
    std::shared_ptr<DeRegisterController>deRegisterController;
    std::shared_ptr<InfoController>infoController;
    std::shared_ptr<NdnProtocol> ndnProtocol;
};
#endif