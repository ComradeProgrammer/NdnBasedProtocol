#ifndef __ADDR_CRONJOB_CONTROLLER_H_
#define __ADDR_CRONJOB_CONTROLLER_H_
#include "AddrController.h"
#include "networkLayerPlus/ndnAddr/dataPack/AddrHelloInterest.h"
class NdnAddrNeighbor;
class NdnAddrAssignmentProtocol;
class AddrCronjobController {
   public:
    AddrCronjobController(NdnAddrAssignmentProtocol* _protocol) : protocol(_protocol){};
    void sendingHelloMessageCronJob(int interfaceIndex);
    void inactiveTimer(NdnAddrNeighbor* neighbor);
    void waitingTimerCronJob(int interfaceIndex);
    void localAddrRequestCronJob(int interfaceIndex);
    void revokeAssignment(int nonce);
   protected:
    NdnAddrAssignmentProtocol* protocol;
};
#endif