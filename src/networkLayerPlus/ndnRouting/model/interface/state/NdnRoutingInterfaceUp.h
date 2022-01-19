#ifndef __NDN_ROUTING_INTERFACE_UP_H_
#define __NDN_ROUTING_INTERFACE_UP_H_
#include "NdnRoutingInterfaceState.h"
class NdnRoutingInterfaceUp : public NdnRoutingInterfaceState {
   public:
    NdnRoutingInterfaceUp(NdnRoutingInterface* interface) : NdnRoutingInterfaceState(interface) {}
    virtual ~NdnRoutingInterfaceUp() = default;
    virtual void processEvent(NdnRoutingInterfaceEventType event);
    virtual NdnRoutingInterfaceStateType getState() { return NdnRoutingInterfaceStateType::UP; }
};
#endif