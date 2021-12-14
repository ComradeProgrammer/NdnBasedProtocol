#ifndef __NDNADDRESSASSIGNMENT_H_
#define __NDNADDRESSASSIGNMENT_H_
#include <mutex>
#include <unordered_map>
#include "util/log/Logger.h"
#include"ndn/ndnPacket/NdnInterest.h"
#include"ndn/ndnPacket/NdnData.h"
#include"ndn/ndnProtocol/NdnProtocol.h"
#include"ndnAddr/protocol/interface/NdnAddrInterface.h"

class NdnAddressAssignment {
    // singleton pattern
   public:
    static std::shared_ptr<NdnAddressAssignment> getNdnAddressAssignment(std::shared_ptr<Logger> _logger = nullptr);

   private:
    NdnAddressAssignment(std::shared_ptr<Logger> _logger = nullptr);
    static std::mutex staticLock;
    static std::shared_ptr<NdnAddressAssignment> staticPtr;

   public:
    void lock();
    void unlock();

    void initialize();

     /**
     * @brief lock WILL be attained in this function.
     */
    void onReceivePacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    /**
     * @brief lock should have been released before call because this function
     * may attain the lock of NdnProtocol object
     */
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);

    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void setRouterID(uint32_t rid) { routerID = rid; }
    /**
     * @brief get the RouterID  lock NEED to be attained before called
     */
    uint32_t getRouterID() { return routerID; }

   private:
    std::shared_ptr<Logger> logger;
    std::mutex objectLock;
    uint32_t routerID;
    std::unordered_map<int,std::shared_ptr<NdnAddrInterface>>interfaces;
};

#endif