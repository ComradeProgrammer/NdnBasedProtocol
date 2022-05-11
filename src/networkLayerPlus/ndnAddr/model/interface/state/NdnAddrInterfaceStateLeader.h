#ifndef __NDN_ADDR_INTERFACE_STATE_LEADER_H_
#define __NDN_ADDR_INTERFACE_STATE_LEADER_H_
#include "NdnAddrInterfaceState.h"
class NdnAddrInterfaceStateLeader : public NdnAddrInterfaceState {
   public:
    NdnAddrInterfaceStateLeader(NdnAddrInterface* _interface) : NdnAddrInterfaceState(_interface) {}
    virtual ~NdnAddrInterfaceStateLeader() = default;
    virtual void processEvent(NdnAddrInterfaceEventType event) override;
    virtual NdnAddrInterfaceStateType getState() override { return NdnAddrInterfaceStateType::LEADER; }
};

#endif