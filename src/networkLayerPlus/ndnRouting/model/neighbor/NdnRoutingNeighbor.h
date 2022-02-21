#ifndef __NDN_ROUTING_NEIGHBOR_H_
#define __NDN_ROUTING_NEIGHBOR_H_
#include <set>

#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborState.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborStateDown.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborStateExchange.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborStateFull.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborStateInit.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborStateLoading.h"

class NdnRoutingInterface;
class NdnRoutingNeighbor : public std::enable_shared_from_this<NdnRoutingNeighbor> {
   public:
    /**
     * @brief Construct a new Ndn Routing Neighbor object
     * @param _root the NdnRoutingInterface which the object belongs to
     */
    NdnRoutingNeighbor(NdnRoutingInterface* _root);
    void processEvent(NeighborEventType e);

   public:
    // getters and setters

    RouterID getRouterID() { return routerID; }
    void setRouterID(RouterID id) { routerID = id; }

    MacAddress getMacAddress() { return macAddr; }
    void setMacAddress(MacAddress address) { macAddr = address; }

    Ipv4Address getIpv4Address() { return ipv4Addr; }
    void setIpv4Address(Ipv4Address addr) { ipv4Addr = addr; }

    Ipv4Address getIpv4Mask() { return ipv4Mask; }
    void setIpv4Mask(Ipv4Address mask) { ipv4Mask = mask; }

    NdnRoutingInterface* getBelongingInterface() { return interface; }

    /**
     * @brief Set the State of neighbor object, should only be called by methods of state object
     */
    void setState(NeighborStateType stateType);
    NeighborStateType getState() { return state->getState(); }

    /**
     * @brief record all active timers so that they can be removed when object is destroyed
     */
    void recordTimer(std::string timerName);
    /**
     * @brief delete timer and remove this name from recorded timer set
     */
    void deleteTimer(std::string timerName);
    void clear();

   private:
    NdnRoutingInterface* interface;
    RouterID routerID;
    MacAddress macAddr;
    Ipv4Address ipv4Addr;
    Ipv4Address ipv4Mask;

    std::shared_ptr<NdnRoutingNeighborState> state;
    // record all active timers
    std::set<std::string> activeTimers;
};

#endif