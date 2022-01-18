#ifndef __NDN_ROUTING_INTERFACE_UP_H_
#define __NDN_ROUTING_INTERFACE_UP_H_
#include "NdnRoutingInterfaceState.h"
class NdnRoutingInterfaceUp : public NdnRoutingInterafaceState {
   public:
    NdnRoutingInterfaceUp(NdnRoutingInterface* interface) : NdnRoutingInterafaceState(interface) {}
    virtual ~NdnRoutingInterfaceUp() = default;
    virtual void processEvent(NdnRoutingInterfaceEventType event);
    virtual NdnRoutingInterfaceStateType getState() { return NdnRoutingInterfaceStateType::UP; }
};
#endif