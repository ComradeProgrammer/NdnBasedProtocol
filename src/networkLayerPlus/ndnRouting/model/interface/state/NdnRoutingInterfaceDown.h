#ifndef __NDN_ROUTING_INTERFACE_DOWN_H_
#define __NDN_ROUTING_INTERFACE_DOWN_H_
#include "NdnRoutingInterfaceState.h"
class NdnRoutingInterfaceDown: public NdnRoutingInterafaceState{
    public:
    NdnRoutingInterfaceDown(NdnRoutingInterface* interface): NdnRoutingInterafaceState(interface){}
    virtual ~NdnRoutingInterfaceDown() = default;
    virtual void processEvent(NdnRoutingInterfaceEventType event);
    virtual NdnRoutingInterfaceStateType getState() { return NdnRoutingInterfaceStateType::DOWN;}

};
#endif