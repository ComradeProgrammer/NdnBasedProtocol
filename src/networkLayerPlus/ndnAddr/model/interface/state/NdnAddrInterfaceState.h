#ifndef __NDN_ADDR_INTERFACE_STATE_H_
#define __NDN_ADDR_INTERFACE_STATE_H_
#include <string>
enum class NdnAddrInterfaceStateType {
    DOWN,
    WAITING,
    LEADER,
    NORMAL,
};

enum class NdnAddrInterfaceEventType {
    INTERFACE_UP,
    WAIT_TIMER_UP,
    NEIGHBOR_CHANGE,
    INTERFACE_DOWN,
};

class NdnAddrInterface;
class NdnAddrInterfaceState {
   public:
    NdnAddrInterfaceState(NdnAddrInterface* _interface) : interface(_interface) {}

    virtual ~NdnAddrInterfaceState() = default;
    virtual void processEvent(NdnAddrInterfaceEventType event) = 0;
    virtual NdnAddrInterfaceStateType getState() = 0;

   protected:
    NdnAddrInterface* interface;
};

std::string getNameForAddrInterfaceStateType(NdnAddrInterfaceStateType state);
std::string getNameForAddrInterfaceEventType(NdnAddrInterfaceEventType event);
#endif