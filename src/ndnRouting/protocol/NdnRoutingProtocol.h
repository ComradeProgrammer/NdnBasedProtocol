#ifndef __NDNROUTINGPROTOCOL_H_
#define __NDNROUTINGPROTOCOL_H_
#include <list>
#include <unordered_map>
#include <vector>

#include "ethernet/interface/NICManager.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndnRouting/dataPack/InfoInterestPack.h"
#include "ndnRouting/dataPack/LsaDataPack.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
#include "ndnRouting/protocol/lsaDatabase/LsaDatabase.h"
#include "util/log/Logger.h"
class LsaDataBase;
// singleton design pattern
class NdnRoutingProtocol {
   public:
    // singleton design implementation.
    static std::shared_ptr<NdnRoutingProtocol> getNdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);

   private:
    NdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);
    static std::shared_ptr<NdnRoutingProtocol> protocol;
    static std::mutex staticLock;

   public:
    // TODO: set adjLsa and rchLsa private
    LsaDataBase database;
    ~NdnRoutingProtocol() = default;
    void initialize();

    void lock();
    void unlock();

    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void setRouterID(uint32_t rid) { routerID = rid; }
    /**
     * @brief get the RouterID  lock NEED to be attained before called
     */
    uint32_t getRouterID() { return routerID; }

    /**
     * @brief lock WILL be attained in this function.
     */
    void onReceivePacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    /**
     * @brief lock should have been released before call because this function
     * may attain the lock of NdnProtocol object
     */
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);

    // get a const reference of adjLsa  lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getAdjLsa() { return database.getAdjLsa(); }
    // get a const reference of rchLsa lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getRchLsa() { return database.getRchLsa(); }

    /**
     * @brief find a by routerid.
     *
     * @return std::shared_ptr<LsaDataPack>. nullptr if not found
     */
    std::shared_ptr<LsaDataPack> findLsa(LinkStateType lsaType, uint32_t routerID);

    void insertLsa(std::shared_ptr<LsaDataPack> lsa) { database.insertLsa(lsa); }
    void rebuildRoutingTable() { database.rebuildRoutingTable(); }
    bool inBroadcastLsaPendingRequestList(LinkStateType lsaType, uint32_t routerID, uint32_t sequenceNum);

    std::shared_ptr<LsaDataPack> generateLsa();

   private:
    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void onReceiveHelloInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest>);
    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void onReceiveDDInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest>);

    void onReceiveDDData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData>);
    void onReceiveLsaInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest>);
    void onReceiveLsaData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData>);
    void onReceiveInfoInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest>);
    void sendBroadcastLsaInterest(LinkStateDigest digest, int interface);

   private:
    std::shared_ptr<Logger> logger;
    std::mutex syncLock;
    uint32_t routerID;
    std::unordered_map<int, std::shared_ptr<NdnRoutingInterface>> interfaces;

    std::list<LinkStateDigest> broadcastLsaPendingRequestList;
};
#endif