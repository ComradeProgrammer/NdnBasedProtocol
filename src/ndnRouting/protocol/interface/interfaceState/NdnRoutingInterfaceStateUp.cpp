#include "NdnRoutingInterfaceStateUp.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
void NdnRoutingInterfaceStateUp::processEvent(NdnRoutingInterfaceEventType event) {
    interface->getLogger()->INFOF(
            "NdnInterfaceState::processEvent, interface %d, current state %s, "
            "event %s",
            interface->getInterfaceID(), "DOWN",
            getNameForInterfaceEventType(event).c_str());
    switch(event){
        case INTERFACE_DOWN:
        interface->clear();
        interface->changeState(DOWN);
    }
}