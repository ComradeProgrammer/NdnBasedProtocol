#ifndef __ADDR_CRONJOB_CONTROLLER_H_
#define __ADDR_CRONJOB_CONTROLLER_H_
#include "AddrController.h"
#include "networkLayerPlus/ndnAddr/dataPack/AddrHelloInterest.h"
class NdnAddrAssignmentProtocol;
class AddrCronjobController {
   public:
    AddrCronjobController(NdnAddrAssignmentProtocol* _protocol) : protocol(_protocol){};
    void sendingHelloMessageCronJob(int interfaceIndex);

    void waitingTimerCronJob(int interfaceIndex);

   protected:
    NdnAddrAssignmentProtocol* protocol;
};
#endif