#include "NdnRoutingInterfaceStateDown.h"

#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
void NdnRoutingInterfaceState::processEvent(
    NdnRoutingInterfaceEventType event) {
    interface->getLogger()->INFOF(
        "NdnInterfaceState::processEvent, interface %d, current state %s, "
        "event %s",
        interface->getInterfaceID(), "DOWN",
        getNameForInterfaceEventType(event).c_str());
    switch (event) {
        case INTERFACE_UP:
            // start HelloMessage Timer

            // switch to Up State

            break;
    }
}