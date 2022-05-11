#ifndef __NDN_ADDR_INTERFACE_STATE_DOWN_H_
#define __NDN_ADDR_INTERFACE_STATE_DOWN_H_
#include "NdnAddrInterfaceState.h"
class NdnAddrInterfaceStateDown : public NdnAddrInterfaceState {
   public:
    NdnAddrInterfaceStateDown(NdnAddrInterface* _interface) : NdnAddrInterfaceState(_interface) {}
    virtual ~NdnAddrInterfaceStateDown() = default;
    virtual void processEvent(NdnAddrInterfaceEventType event) override;
    virtual NdnAddrInterfaceStateType getState() override { return NdnAddrInterfaceStateType::DOWN; }
};

#endif