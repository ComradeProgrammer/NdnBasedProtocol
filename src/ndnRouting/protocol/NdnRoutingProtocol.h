#ifndef __NDNROUTINGPROTOCOL_H_
#define __NDNROUTINGPROTOCOL_H_
#include <unordered_map>
#include<vector>
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
#include"ndnRouting/dataPack/LsaDataPack.h"
#include "util/log/Logger.h"
// singleton design pattern
class NdnRoutingProtocol {
   public:
    // singleton design implementation.
    static std::shared_ptr<NdnRoutingProtocol> getNdnRoutingProtocol(
        std::shared_ptr<Logger> _logger = nullptr);

   private:
    NdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);
    static std::shared_ptr<NdnRoutingProtocol> protocol;
    static std::mutex staticLock;

   public:
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
    void onReceivePacket(int interfaceIndex, MacAddress sourceMac,
                         std::shared_ptr<NdnPacket> packet);
    /**
     * @brief lock should have been released before call because this function
     * may attain the lock of NdnProtocol object
     */
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);

    //get a const reference of adjLsa
    const std::vector<LsaDataPack>& getAdjLsa(){return adjLsa;}
    //get a const reference of rchLsa
    const std::vector<LsaDataPack>& getRchLsa(){return rchLsa;}

   private:
    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void onReceiveHelloInterest(int interfaceIndex, MacAddress sourceMac,
                                std::shared_ptr<NdnInterest>);
    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void onReceiveDDInterest(int interfaceIndex, MacAddress sourceMac,
                                std::shared_ptr<NdnInterest>);

    
   private:
    std::shared_ptr<Logger> logger;
    std::mutex syncLock;
    uint32_t routerID;
    std::unordered_map<int, std::shared_ptr<NdnRoutingInterface>> interfaces;
    std::vector<LsaDataPack>adjLsa;
    std::vector<LsaDataPack>rchLsa;

};
#endif