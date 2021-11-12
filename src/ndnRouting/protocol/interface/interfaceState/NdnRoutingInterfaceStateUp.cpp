#include "NdnRoutingInterfaceStateUp.h"

#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
using namespace std;
void NdnRoutingInterfaceStateUp::processEvent(NdnRoutingInterfaceEventType event) {
    interface->getLogger()->INFOF(
        "NdnInterfaceState::processEvent, interface %d, current state %s, "
        "event %s",
        interface->getInterfaceID(), "DOWN", getNameForInterfaceEventType(event).c_str());
    switch (event) {
        case INTERFACE_DOWN:
            // remove the timer
            Timer::GetTimer()->cancelTimer("hello_timer_" + to_string(interface->getInterfaceID()));
            interface->clear();
            interface->changeState(NdnRoutingInterfaceStateType::DOWN);
    }
}