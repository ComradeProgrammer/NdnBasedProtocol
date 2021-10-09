#ifndef __NDNROUTINGINTERFACESTATE_H_
#define __NDNROUTINGINTERFACESTATE_H_
#include <string>
enum NdnRoutingInterfaceStateType {
    DOWN = 0,
    UP,
};

enum NdnRoutingInterfaceEventType {
    INTERFACE_UP = 0,
    HELLO_TIMER,
    INFO_TIMER,
    INTERFACE_DOWN,
};

class NdnRoutingInterface;
// state machine design pattern
// abstract class
class NdnRoutingInterfaceState {
   public:
    NdnRoutingInterfaceState(NdnRoutingInterface* context)
        : interface(context) {}
    virtual ~NdnRoutingInterfaceState() = default;
    virtual void processEvent(NdnRoutingInterfaceEventType) = 0;
    virtual NdnRoutingInterfaceStateType getState() = 0;

   protected:
    NdnRoutingInterface* interface;
};
std::string getNameForInterfaceStateType(NdnRoutingInterfaceStateType state);
std::string getNameForInterfaceEventType(NdnRoutingInterfaceEventType event);
#endif