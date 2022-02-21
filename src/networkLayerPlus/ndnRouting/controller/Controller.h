#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
class NdnRoutingProtocol;
class Controller {
   public:
    Controller(NdnRoutingProtocol* _protocol) : protocol(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) = 0;
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) = 0;

   protected:
    NdnRoutingProtocol* protocol;
};
#endif