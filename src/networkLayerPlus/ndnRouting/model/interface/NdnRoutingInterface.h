#ifndef __NDN_ROUTING_INTERFACE_H_
#define __NDN_ROUTING_INTERFACE_H_
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
class NdnRoutingInterface : public NicObserverInterface {
   public:
    NdnRoutingInterface(NdnRoutingProtocol* _protocol);

    //implementing NicObserverInterface
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

    void setState(NdnRoutingInterfaceStateType newStateType);
    NdnRoutingInterfaceStateType getState();

    NdnRoutingProtocol* getProtocol(){return protocol;}

    /**
     * @brief wipe out all data stored in this object. lock of protocol object should have been required.
     *
     */
    void clear();

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