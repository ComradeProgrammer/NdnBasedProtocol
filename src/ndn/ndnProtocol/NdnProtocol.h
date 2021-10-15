#ifndef __NDNPROTOCOL_H_
#define __NDNPROTOCOL_H_
#include <memory>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "ndn/ndnProtocol/components/cs/ContentStore.h"
#include "ndn/ndnProtocol/components/deadNonceList/DeadNonceList.h"
#include "ndn/ndnProtocol/components/pit/Pit.h"
#include "ndn/ndnProtocol/strategies/forwarddata/ForwardDataStrategyBase.h"
#include "ndn/ndnProtocol/strategies/nexthops/NextHopStrategyBase.h"
#include "util/util.h"
class NdnProtocol {
   public:
    static std::shared_ptr<NdnProtocol> getNdnProtocol(
        std::shared_ptr<Logger> log = nullptr);

   private:
    static std::shared_ptr<NdnProtocol> ndnProtocol;
    static std::mutex staticLock;

    /**
     * @brief default constructor \n  default nexthop strategy:
     * NextHopStrategyBroadcastToEveryoneElse
     */
    NdnProtocol(std::shared_ptr<Logger> log = nullptr);

   public:
    // this object is forbidden to be coppied, thus copy constructor is deleted
    // explicitly
    NdnProtocol(const NdnProtocol&) = delete;

    // deconstructor
    virtual ~NdnProtocol() = default;

    void onIncomingPacket(int interfaceIndex, MacAddress sourceMac,
                          std::shared_ptr<NdnPacket> packet);

    static void registerUpperLayerProtocol(
        int protocol,
        std::function<void(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnPacket>)>
            messageHandler);
    static std::unordered_map<
        int, std::function<void(int interfaceIndex, MacAddress sourceMac,
                                std::shared_ptr<NdnPacket>)>>
    getRegisteredUpperLayerProtocol();

   private:
    // interest pipeline functions

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
    void onOutgoingInterest(int interfaceIndex, MacAddress sourceMac,
                            std::shared_ptr<NdnInterest> interest,
                            std::vector<std::pair<int, MacAddress>> faces);
    // data pipeline functions
    void onIncomingData(int interfaceIndex, MacAddress sourcecMac,
                        std::shared_ptr<NdnData> data);
    void onDataUnsolicited(int interfaceIndex, MacAddress sourcecMac,
                           std::shared_ptr<NdnData> data);
    void onOutgoingData(int interfaceIndex, MacAddress sourceMac,
                        std::shared_ptr<NdnData> data,
                        std::vector<std::pair<int, MacAddress>> faces);

   private:
    void sendPacket(int targetInterfaceIndex, MacAddress destination,
                    std::shared_ptr<NdnPacket> packet, int sourceInterfaceIndex);

   private:
    static std::unordered_map<
        int, std::function<void(int interfaceIndex, MacAddress sourceMac,
                                std::shared_ptr<NdnPacket>)>>
        registeredProtocol;

    std::shared_ptr<Logger> logger = nullptr;
    std::shared_ptr<DeadNonceList> deadNonceList = nullptr;
    std::shared_ptr<Pit> pit = nullptr;
    std::shared_ptr<ContentStore> cs = nullptr;
    // lock must be required before any query or modification is made
    std::mutex protocolLock;
    // nexthop strategy
    std::shared_ptr<NextHopStrategyBase> nextHopStrategy;
    std::shared_ptr<ForwardDataStrategyBase> forwardDataStrategy;
};

#endif