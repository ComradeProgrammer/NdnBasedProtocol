#ifndef __ADDR_CHAIN_CONTROLLER_H_
#define __ADDR_CHAIN_CONTROLLER_H_
#include "AddrController.h"
#include"util/blockchain/BlockChain.h"
class AddrChainController : public AddrController {
   public:
    AddrChainController(NdnAddrAssignmentProtocol* _protocol) : AddrController(_protocol) {}
    virtual void onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet);
    virtual void onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet);
};
#endif