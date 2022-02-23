#include "NdnRoutingInterfaceDown.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "util/log/Logger.h"
#include "util/timer/Timer.h"
using namespace std;
void NdnRoutingInterfaceDown::processEvent(NdnRoutingInterfaceEventType event) {
    LOGGER->INFOF(2,
        "NdnInterfaceState::processEvent, interface %d, current state %s, "
        "event %s",
        interface->getInterfaceID(), "DOWN", getNameForInterfaceEventType(event).c_str());
    switch (event) {
        case NdnRoutingInterfaceEventType::INTERFACE_UP:
            // switch to Up State
            interface->setState(NdnRoutingInterfaceStateType::UP);
            // start HelloMessage Timer
            NdnRoutingInterface* intf = interface;
            IOC->getTimer()->startTimer("hello_timer_" + to_string(interface->getInterfaceID()), NDNROUTING_HELLOINTERVAL * 1000, [intf](string) -> bool {
                intf->getProtocol()->getCrobJobHandler()->sendingHelloMessageCronJob(intf->getInterfaceID());
                return true;
            });
            break;
    }
}