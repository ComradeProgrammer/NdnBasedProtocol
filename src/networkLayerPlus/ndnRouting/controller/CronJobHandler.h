#ifndef __CRONJOB_HANDLER_H_
#define __CRONJOB_HANDLER_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "util/declaration.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
class NdnRoutingProtocol;

class CronJobHandler {
   public:
    CronJobHandler(NdnRoutingProtocol* _protocol) : protocol(_protocol) {}
    void sendingHelloMessageCronJob(int interfaceIndex);

   protected:
    NdnRoutingProtocol* protocol;
};
#endif