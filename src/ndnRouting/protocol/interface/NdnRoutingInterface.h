#ifndef __NDNROUTINGINTERFACE_H_
#define __NDNROUTINGINTERFACE_H_
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ethernet/interface/NIC.h"
#include "ndnRouting/dataPack/HelloInterestPack.h"
#include "ndnRouting/protocol/NdnRoutingConstant.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceState.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceStateDown.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceStateUp.h"
#include "ndnRouting/protocol/interface/neighbor/NdnRoutingNeighbor.h"
class NdnRoutingInterface {
   public:
    NdnRoutingInterface(NIC nic, std::shared_ptr<Logger> _logger = nullptr);

    // getters and setterrs

    // read-only attributes doesn't need mutex lock;
    std::string getName() { return name; }
    int getInterfaceID() { return interfaceID; }
    MacAddress getMacAddress() { return macAddress; }
    std::shared_ptr<Logger> getLogger() { return logger; }

    /**
     * @brief lock of protocol object should be have been acquired
     */
    void processStateEvent(NdnRoutingInterfaceEventType event);
    /**
     * @brief lock of protocol object should be have been acquired
     */
    void changeState(NdnRoutingInterfaceStateType newState);
    /**
     * @brief lock of protocol object should be have been acquired
     */
    std::shared_ptr<NdnRoutingInterfaceState> getState() { return state; }

    /**
     * @brief send hello interest through this interface.lock of protocol object will be acquired
     */
    void sendHelloInterests();
    /**
     * @brief handle received hello interest through this interface.lock of protocol object will be acquired
     */
    void onReceiveHelloInterest(MacAddress sourceAddr, std::shared_ptr<NdnInterest> interest);
    /**
     * @brief handle received dd interest through this interface.lock of protocol object will be acquired
     */
    void onReceiveDDInterest(MacAddress sourceAddr, std::shared_ptr<NdnInterest> interest);

    /**
     * @brief wipe out all data stored in this object. lock of protocol object should have been required.
     * 
     */
    void clear();

    private:
    /**
     * @brief Get the Neighbor By Mac address.
     * 
     * @return std::shared_ptr<NdnRoutingNeighbor>, pointer to the neighbor. return nullptr if not found
     */
    std::shared_ptr<NdnRoutingNeighbor> getNeighborByMac(MacAddress mac);

   private:
    std::shared_ptr<Logger> logger;
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    Ipv4Address ipv4Addr;
    Ipv4Address ipv4Mask;
    std::shared_ptr<NdnRoutingInterfaceState> state;
    //router id ->Neighbor
    std::unordered_map<uint32_t,std::shared_ptr<NdnRoutingNeighbor>>neighbors;
};
#endif