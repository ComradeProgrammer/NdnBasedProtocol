#include "NdnRoutingInterfaceStateDown.h"

#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
using namespace std;
void NdnRoutingInterfaceStateDown::processEvent(NdnRoutingInterfaceEventType event) {
    interface->getLogger()->INFOF(
        "NdnInterfaceState::processEvent, interface %d, current state %s, "
        "event %s",
        interface->getInterfaceID(), "DOWN", getNameForInterfaceEventType(event).c_str());
    switch (event) {
        case INTERFACE_UP:
            // switch to Up State
            interface->changeState(NdnRoutingInterfaceStateType::UP);
            // start HelloMessage Timer
            startSendingHelloMessage();
            break;
    }
}

void NdnRoutingInterfaceStateDown::startSendingHelloMessage() {
    // sleep for a short random time to avoid that all instances sens hello
    // int randMillSec=rand()%200;
    // this_thread::sleep_for(chrono::milliseconds(randMillSec));
    auto timer = Timer::GetTimer();
    auto tmp = interface;
    timer->startTimer("hello_timer_" + to_string(interface->getInterfaceID()), NDNROUTING_HELLOINTERVAL * 1000,
                      [tmp](string) -> bool {
                          tmp->sendHelloInterests();
                          return true;
                      });
}
