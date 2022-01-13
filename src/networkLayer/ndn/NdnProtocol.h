#ifndef __NDN_PROTOCOL_H_
#define __NDN_PROTOCOL_H_
#include "networkLayer/NetworkLayerProtocol.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/component/cs/ContentStore.h"
#include "networkLayer/ndn/component/deadNonceList/DeadNonceList.h"
#include "networkLayer/ndn/component/pit/Pit.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayer/ndn/strategy/forwarddata/MyForwardStrategy.h"
#include "networkLayer/ndn/strategy/forwarddata/ForwardDataStrategyBase.h"
#include "networkLayer/ndn/strategy/nexthops/MyNextHopStrategy.h"
#include "networkLayer/ndn/strategy/nexthops/NextHopStrategyBase.h"
#include "linkLayer/transmitter/Transmitter.h"

class NdnProtocol : public NetworkLayerProtocol {
   public:
    NdnProtocol();
    NdnProtocol(const NdnProtocol&) = delete;
    virtual ~NdnProtocol() = default;
    /**
     * @brief entrance handler of the protocol for packet from lower layer.
     */
    virtual void onReceiveEthernetPacket(int sourceInterface, std::shared_ptr<EthernetPacket>) override;
    /**
     * @brief direct entrance handler of the protocol for packet from upper layer.
     */
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    /**
     * @brief register an upper layer protocol.
     * 
     * @param interfaceID  should be minus in order to distinguish them with real nic interface
     * @param protocol pointer of protocol object
     */
    void registerUpperLayerProtocol(int interfaceID,NdnProtocolPlus* protocol);

   protected:
    //lock will be attained and released
    void onIncomingInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest);
    //caller has attained lock, won't release the lock
    void onInterestLoop(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest);
    //caller has attained lock, won't release the lock
    void onContentStoreHit(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest);
    //caller has attained lock, won't release the lock
    void onContentStoreMiss(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest);
    //caller has attained lock, won't release the lock
    void onInterestFinalize(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest);
    //caller has attained lock, won't release the lock
    void onOutgoingInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest,
                            std::vector<std::pair<int, MacAddress>> faces);
    // data pipeline functions
    //lock will be attained and released
    void onIncomingData(int interfaceIndex, MacAddress sourcecMac, std::shared_ptr<NdnData> data);
    //caller has attained lock, won't release the lock
    void onDataUnsolicited(int interfaceIndex, MacAddress sourcecMac, std::shared_ptr<NdnData> data);
    //caller has attained lock, won't release the lock
    void onOutgoingData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data,
    //caller has attained lock, won't release the lock
                        std::vector<std::pair<int, MacAddress>> faces);
    //caller has attained lock, won't release the lock
    void sendPacket(int targetInterfaceIndex, MacAddress destination, std::shared_ptr<NdnPacket> packet,
                    int sourceInterfaceIndex,MacAddress sourceMac);


    //controls whether a pit should be excluded from pit
    bool excludedFromPit(std::shared_ptr<NdnPacket> interest);

    private:
    std::shared_ptr<DeadNonceList> deadNonceList = nullptr;
    std::shared_ptr<Pit> pit = nullptr;
    std::shared_ptr<ContentStore> cs = nullptr;
    std::unordered_map<int,NdnProtocolPlus*>upperLayerProtocols;
    std::shared_ptr<NextHopStrategyBase> nextHopStrategy;
    std::shared_ptr<ForwardDataStrategyBase> forwardDataStrategy;
    // lock must be required before any query or modification is made
    std::mutex protocolLock;
};

#endif