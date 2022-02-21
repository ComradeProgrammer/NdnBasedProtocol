#ifndef __NDN_LAYER_PLUS_PROTOCOL_H_
#define __NDN_LAYER_PLUS_PROTOCOL_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ndn/ndnPacket/NdnPacket.h"
class NdnProtocolPlus {
   public:
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) = 0;
};
#endif