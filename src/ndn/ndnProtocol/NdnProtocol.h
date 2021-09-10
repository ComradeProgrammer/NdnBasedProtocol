#ifndef __NDNPROTOCOL_H_
#define __NDNPROTOCOL_H_
#include <memory>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/components/cs/ContentStore.h"
#include "ndn/ndnProtocol/components/deadNonceList/DeadNonceList.h"
#include "ndn/ndnProtocol/components/pit/Pit.h"
#include "util/util.h"
#include"ndn/ndnProtocol/strategies/nexthops/NextHopStrategyBase.h"
#include"ndn/ndnProtocol/strategies/nexthops/NextHopStrategyBroadcastToEveryoneElse.h"
class NdnProtocol {
   public:
    /**
     * @brief default constructor \n  default nexthop strategy: NextHopStrategyBroadcastToEveryoneElse
     * @param log 
     * @return * default* 
     */

    NdnProtocol(std::shared_ptr<Logger> log = nullptr);

    // this object is forbidden to be coppied, thus copy constructor is deleted
    // explicitly
    NdnProtocol(const NdnProtocol&) = delete;

    // deconstructor
    virtual ~NdnProtocol() = default;

   private:
    // pipeline functions

    void onIncomingInterest(int interfaceIndex, MacAddress sourceMac,
                            std::shared_ptr<NdnInterest> interest);
    void onInterestLoop(int interfaceIndex, MacAddress sourceMac,
                        std::shared_ptr<NdnInterest> interest);
    /**
     * @brief caller has attained lock, won't release the lock
    */
    void onContentStoreHit(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnInterest> interest);
    /**
     * @brief caller has attained lock, won't release the lock
    */
    void onContentStoreMiss(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnInterest> interest);
    /**
     * @brief caller has attained lock, won't release the lock
    */                     
    void onInterestFinalize(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnInterest> interest);
    /**
     * @brief caller has attained lock, won't release the lock
    */      
    void onOutgoingInterest(int interfaceIndex, MacAddress sourceMac,std::shared_ptr<NdnInterest>interest, std::vector<std::pair<int,MacAddress>>faces);

   private:
    std::shared_ptr<Logger> logger = nullptr;
    std::shared_ptr<DeadNonceList> deadNonceList = nullptr;
    std::shared_ptr<Pit> pit = nullptr;
    std::shared_ptr<ContentStore> cs = nullptr;
    // lock must be required before any query or modification is made
    std::mutex protocolLock;
    //nexthop strategy
    std::shared_ptr<NextHopStrategyBase>nextHopStrategy;
};

#endif