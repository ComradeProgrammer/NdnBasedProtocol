#ifndef __NDNROUTINGINTERFACE_H_
#define __NDNROUTINGINTERFACE_H_
#include "ethernet/interface/NIC.h"
#include "ndnRouting/dataPack/HelloInterestPack.h"
#include "ndnRouting/protocol/NdnRoutingConstant.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceState.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceStateDown.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceStateUp.h"
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
     * @brief wipe out all data stored in this object. lock of protocol object should have been required.
     * 
     */
    void clear();

   private:
    std::shared_ptr<Logger> logger;
    std::string name;
    int interfaceID;
    MacAddress macAddress;

    Ipv4Address ipv4Addr;
    Ipv4Address ipv4Mask;
    std::shared_ptr<NdnRoutingInterfaceState> state;
};
#endif