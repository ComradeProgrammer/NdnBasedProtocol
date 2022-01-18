#ifndef __NDN_ROUTING_INTERFACESTATE_H_
#define __NDN_ROUTING_INTERFACESTATE_H_
#include <string>

class NdnRoutingInterface;
enum NdnRoutingInterfaceStateType {
    DOWN = 0,
    UP,
};

enum NdnRoutingInterfaceEventType {
    INTERFACE_UP = 0,
    INTERFACE_DOWN,
};

class NdnRoutingInterafaceState {
   public:
    NdnRoutingInterafaceState(NdnRoutingInterface* _interface):interface(_interface){}
    virtual ~NdnRoutingInterafaceState() = default;
    virtual void processEvent(NdnRoutingInterfaceEventType event) = 0;
    virtual NdnRoutingInterfaceStateType getState() = 0;
    protected:
    NdnRoutingInterface* interface;
};

std::string getNameForInterfaceStateType(NdnRoutingInterfaceStateType state);
std::string getNameForInterfaceEventType(NdnRoutingInterfaceEventType event);
#endif