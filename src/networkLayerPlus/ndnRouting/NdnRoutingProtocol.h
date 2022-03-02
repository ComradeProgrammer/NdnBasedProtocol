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
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/dataPack/RegisterInterestPack.h"
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
    /**
     * @brief get Neighbor object by Router ID
     * @return std::shared_ptr<NdnRoutingNeighbor>, nullptr if not found
     */
    std::shared_ptr<NdnRoutingNeighbor> getNeighborByRouterID(RouterID id);

    /**
     * @brief generate new lsa and replace the old one if it exists
     *
     * @return std::shared_ptr<LsaDataPack> new lsa
     */
    std::shared_ptr<LsaDataPack> generateLsa();

    bool inBroadcastLsaPendingRequestList(LinkStateType lsaType, RouterID routerID, uint32_t sequenceNum);

    /**
     * @brief recalculate all parents for all roots, and send register packets and deregister packets;
     */
    void registerParents();
    long sendRegisterPacket(RouterID root, RouterID parent);
    long sendDeregisterPacket(RouterID root, RouterID parent);

    long getLastRegistrationTime(RouterID root, RouterID son);

    void lock() { mutexLock->lock(); }
    void unlock() { mutexLock->unlock(); }

    friend class Controller;
    friend class HelloController;
    friend class CronJobHandler;
    friend class DDController;
    friend class LsaController;
    friend class RegisterController;

   private:
    std::shared_ptr<std::mutex> mutexLock;
    RouterID routerID;
    std::unordered_map<int, std::shared_ptr<NdnRoutingInterface>> interfaces;
    std::shared_ptr<LsaDatabase> database;
    std::list<LinkStateDigest> broadcastLsaPendingRequestList;

    // root->sons () root->sons 向本路由器登记为以root为根的儿子的节点
    std::unordered_map<RouterID, std::vector<RouterID>> registeredSons;
    // timestamp we have seen(when son come to register/deregister)
    //[root][son]->time 对以root为根的树，son以儿子身份向本路由器最后一次注册/解除注册的时间
    std::unordered_map<RouterID, std::unordered_map<RouterID, long>> lastOperationTime;

    // root->parents 我们在以root为根的树上向谁注册为儿子了
    std::unordered_map<RouterID, RouterID> registeredParents;

    std::shared_ptr<CronJobHandler> cronJobHandler;
    std::shared_ptr<HelloController> helloController;
    std::shared_ptr<DDController> ddController;
    std::shared_ptr<LsaController> lsaController;
    std::shared_ptr<RegisterController> registerController;
    std::shared_ptr<NdnProtocol> ndnProtocol;
};
#endif