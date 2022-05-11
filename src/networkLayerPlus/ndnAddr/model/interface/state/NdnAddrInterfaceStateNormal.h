#ifndef __NDN_ADDR_INTERFACE_STATE_NORMAL_H_
#define __NDN_ADDR_INTERFACE_STATE_NORMAL_H_
#include "NdnAddrInterfaceState.h"
class NdnAddrInterfaceStateNormal : public NdnAddrInterfaceState {
   public:
    NdnAddrInterfaceStateNormal(NdnAddrInterface* _interface) : NdnAddrInterfaceState(_interface) {}
    virtual ~NdnAddrInterfaceStateNormal() = default;
    virtual void processEvent(NdnAddrInterfaceEventType event) override;
    virtual NdnAddrInterfaceStateType getState() override { return NdnAddrInterfaceStateType::NORMAL; }
};

#endif