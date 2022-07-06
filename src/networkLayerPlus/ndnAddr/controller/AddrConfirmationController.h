#ifndef __ADDR_CONFIRMATION_CONTROLLER_H_
#define __ADDR_CONFIRMATION_CONTROLLER_H_
#include "AddrController.h"
class AddrConfirmationController : public AddrController {
   public:
    AddrConfirmationController(NdnAddrAssignmentProtocol* _protocol) : AddrController(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet);
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet);
};
#endif