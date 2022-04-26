#ifndef __NDN_ROUTING_ACL_DATA_H_
#define __NDN_ROUTING_ACL_DATA_H_
#include "linkLayer/MacAddress.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"

enum PacketType { INTEREST, DATA };
enum PacketKind {HELLO, DD,LSA,INFO,REGISTER,DEREGISTER};
class NdnRoutingAclData {
   public:
    RouterID sourceRouterID;
    PacketType packetType;
    std::string packetName;
    MacAddress sourceMacAddress;
    int interfaceIndex;
};
#endif