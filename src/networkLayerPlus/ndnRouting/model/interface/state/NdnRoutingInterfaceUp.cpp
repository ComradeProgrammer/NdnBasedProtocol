#include"NdnRoutingInterfaceUp.h"
#include "util/log/Logger.h"
#include"networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include"networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include"util/timer/Timer.h"
using namespace std;
void NdnRoutingInterfaceUp::processEvent(NdnRoutingInterfaceEventType event){
     LOGGER->INFOF(
        "NdnInterfaceState::processEvent, interface %d, current state %s, "
        "event %s",
        interface->getInterfaceID(), "UP", getNameForInterfaceEventType(event).c_str());
    switch (event) {
        case NdnRoutingInterfaceEventType::INTERFACE_DOWN:
            // remove the timer
            IOC->getTimer()->cancelTimer("hello_timer_" + to_string(interface->getInterfaceID()));
            interface->clear();
            interface->setState(NdnRoutingInterfaceStateType::DOWN);
    }
}