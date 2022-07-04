#ifndef __ADDR_REQUEST_CONTROLLER_H_
#define __ADDR_REQUEST_CONTROLLER_H_
#include "AddrController.h"
#include "networkLayerPlus/ndnAddr/dataPack/AddrRequestData.h"
class AddrRequestController : public AddrController {
   public:
    AddrRequestController(NdnAddrAssignmentProtocol* _protocol) : AddrController(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet);
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet);
};
#endif