#include "NdnAddrInterfaceStateDown.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
#include "networkLayerPlus/ndnAddr/model/interface/NdnAddrInterface.h"
#include "util/declaration.h"
using namespace std;
void NdnAddrInterfaceStateDown::processEvent(NdnAddrInterfaceEventType event) {
    switch (event) {
        case NdnAddrInterfaceEventType::INTERFACE_UP: {
            NdnAddrInterface* intf = interface;
            RouterID id = interface->getInterfaceID();
            if (interface != nullptr) {
                // start sending hello
                IOC->getTimer()->startTimer("hello_timer_" + to_string(id), NDNADDR_HELLOINTERVAL * 1000, [intf](string) -> bool {
                    intf->getProtocol()->getCronjobHandler()->sendingHelloMessageCronJob(intf->getInterfaceID());
                    return true;
                });
                // start WAITING

                IOC->getTimer()->startTimer("waiting_timer_" + to_string(id), NDNADDR_WAITINTERVAL * 1000, [intf](string) -> bool {
                    intf->getProtocol()->getCronjobHandler()->waitingTimerCronJob(intf->getInterfaceID());
                    return false;
                });
            }
            interface->setState(NdnAddrInterfaceStateType::WAITING);
        }
    }
}