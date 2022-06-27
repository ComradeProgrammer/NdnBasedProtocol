#ifndef __NDN_ROUTING_INTERFACE_H_
#define __NDN_ROUTING_INTERFACE_H_
#include <list>
#include <string>
#include <unordered_map>

#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/model/interface/state/NdnRoutingInterfaceDown.h"
#include "networkLayerPlus/ndnRouting/model/interface/state/NdnRoutingInterfaceState.h"
#include "networkLayerPlus/ndnRouting/model/interface/state/NdnRoutingInterfaceUp.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"
#include "physicalLayer/nic/NicObserverInterface.h"

class NdnRoutingProtocol;
class NdnRoutingInterface : public NicObserverInterface, public std::enable_shared_from_this<NdnRoutingInterface> {
   public:
    NdnRoutingInterface(NdnRoutingProtocol* _protocol);

    // implementing NicObserverInterface
    virtual void onEventHappen(int interfaceID, NICEvent event) override;

    void processInterfaceEvent(NdnRoutingInterfaceEventType event);

    std::string getName() { return name; }
    void setName(std::string _name) { name = _name; }

    int getInterfaceID() { return interfaceID; }
    void setInterfaceID(int _interfaceID) { interfaceID = _interfaceID; }

    MacAddress getMacAddress() { return macAddress; }
    void setMacAddress(MacAddress address) { macAddress = address; }

    Ipv4Address getIpv4Address() { return ipv4Addr; }
    void setIpv4Address(Ipv4Address addr) { ipv4Addr = addr; }

    Ipv4Address getIpv4Mask() { return ipv4Mask; }
    void setIpv4Mask(Ipv4Address mask) { ipv4Mask = mask; }

    int getCost() { return cost; }
    void setCost(int _cost) { cost = _cost; }

    /**
     * @brief switch to a new state. This function is supposed to be called only by state objects
     */
    void setState(NdnRoutingInterfaceStateType newStateType);
    NdnRoutingInterfaceStateType getState();

    NdnRoutingProtocol* getProtocol() { return protocol; }

    /**
     * @brief get Neighbor object by Router ID
     * @return std::shared_ptr<NdnRoutingNeighbor>, nullptr if not found
     */
    std::shared_ptr<NdnRoutingNeighbor> getNeighborByRouterID(RouterID rid);

    /**
     * @brief Get the Neighbor By MacAddress
     *
     * @return std::shared_ptr<NdnRoutingNeighbor> nullptr if not found
     */
    std::shared_ptr<NdnRoutingNeighbor> getNeighborByMac(MacAddress mac);
    std::unordered_map<RouterID, std::shared_ptr<NdnRoutingNeighbor>> getNeighbors() { return neighbors; }
    void addNeighbor(std::shared_ptr<NdnRoutingNeighbor> neighbor);

    /**
     * @brief wipe out all data stored in this object. lock of protocol object should have been required.
     *
     */
    void clear();

    friend class CronJobHandler;

   private:
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    Ipv4Address ipv4Addr;
    Ipv4Address ipv4Mask;
    int cost = 1;
    std::unordered_map<RouterID, std::shared_ptr<NdnRoutingNeighbor>> neighbors;
    NdnRoutingProtocol* protocol;
    std::shared_ptr<NdnRoutingInterfaceState> state;
};

#endif