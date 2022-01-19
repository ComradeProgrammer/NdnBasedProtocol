#ifndef __NDN_ROUTING_NEIGHBOR_H_
#define __NDN_ROUTING_NEIGHBOR_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include"networkLayerPlus/ndnRouting/model/neighbor/state/NdnRoutingNeighborState.h"
class NdnRoutingNeighbor {
   public:
   void processEvent(NeighborEventType e);
   private:
    RouterID routerID;
    MacAddress macAddress;
    Ipv4Address ipAddress;
    Ipv4Address ipMask;

};
#endif