#ifndef __NDN_ROUTING_INTERFACE_DOWN_H_
#define __NDN_ROUTING_INTERFACE_DOWN_H_
#include<string>
#include "NdnRoutingInterfaceState.h"
#include "ioc.h"
#include"util/declaration.h"
class NdnRoutingInterfaceDown : public NdnRoutingInterfaceState {
   public:
    NdnRoutingInterfaceDown(NdnRoutingInterface* interface) : NdnRoutingInterfaceState(interface) {}
    virtual ~NdnRoutingInterfaceDown() = default;
    virtual void processEvent(NdnRoutingInterfaceEventType event);
    virtual NdnRoutingInterfaceStateType getState() { return NdnRoutingInterfaceStateType::DOWN; }
};
#endif