#ifndef __NDN_ADDR_INTERFACE_STATE_WAITING_H_
#define __NDN_ADDR_INTERFACE_STATE_WAITING_H_
#include "NdnAddrInterfaceState.h"
class NdnAddrInterfaceStateWaiting : public NdnAddrInterfaceState {
   public:
    NdnAddrInterfaceStateWaiting(NdnAddrInterface* _interface) : NdnAddrInterfaceState(_interface) {}
    virtual ~NdnAddrInterfaceStateWaiting() = default;
    virtual void processEvent(NdnAddrInterfaceEventType event) override;
    virtual NdnAddrInterfaceStateType getState() override { return NdnAddrInterfaceStateType::WAITING; }
};
#endif