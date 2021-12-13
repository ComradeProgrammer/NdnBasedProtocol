#ifndef __NDNADDRESSASSIGNMENT_H_
#define __NDNADDRESSASSIGNMENT_H_
#include <mutex>

#include "util/log/Logger.h"
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
};

#endif