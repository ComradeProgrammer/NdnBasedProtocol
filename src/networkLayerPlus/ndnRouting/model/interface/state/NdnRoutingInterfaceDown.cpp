#include "NdnRoutingInterfaceDown.h"
#include "util/log/Logger.h"
#include"networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include"networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include"util/timer/Timer.h"
using namespace std;
void NdnRoutingInterfaceDown::processEvent(NdnRoutingInterfaceEventType event) {
    LOGGER->INFOF(
        "NdnInterfaceState::processEvent, interface %d, current state %s, "
        "event %s",
        interface->getInterfaceID(), "DOWN", getNameForInterfaceEventType(event).c_str());
    switch (event) {
        case NdnRoutingInterfaceEventType::INTERFACE_UP:
            // switch to Up State
            interface->setState(NdnRoutingInterfaceStateType::UP);
            // start HelloMessage Timer
            IOC->getTimer()->startTimer("hello_timer_" + to_string(interface->getInterfaceID()), NDNROUTING_HELLOINTERVAL * 1000,
                    [this](string) -> bool {
                        interface->getProtocol()->getCrobJobHandler()->sendingHelloMessageCronJob(interface->getInterfaceID());
                        return true;
                    });
            break;
    }
}