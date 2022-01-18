#ifndef __NDN_ROUTING_INTERFACE_H_
#define __NDN_ROUTING_INTERFACE_H_
#include<string>
#include<unordered_map>
#include"networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
#include"networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"
#include "linkLayer/MacAddress.h"
#include"networkLayer/ip/Ipv4Address.h"

class NdnRoutingProtocol;
class NdnRoutingInterface{
    public:

    private:
    std::string name;
    int interfaceID;
    Ipv4Address ipv4Addr;
    Ipv4Address ipv4Mask;
    int cost=1;
    std::unordered_map<RouterID,std::shared_ptr<NdnRoutingNeighbor>>neighbors;
    NdnRoutingProtocol* protocol;

};

#endif