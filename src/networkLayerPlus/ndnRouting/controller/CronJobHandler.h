#ifndef __CRONJOB_HANDLER_H_
#define __CRONJOB_HANDLER_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "util/declaration.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"

class NdnRoutingProtocol;

class CronJobHandler {
   public:
    CronJobHandler(NdnRoutingProtocol* _protocol) : protocol(_protocol) {}

    //called in interface down state object
    void sendingHelloMessageCronJob(int interfaceIndex);

    void neighborInactivityCronJob(NdnRoutingNeighbor*neighbor);

   protected:
    NdnRoutingProtocol* protocol;
};
#endif