#include "NdnAddrInterfaceStateDown.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
#include "networkLayerPlus/ndnAddr/model/interface/NdnAddrInterface.h"
#include "util/declaration.h"
using namespace std;
void NdnAddrInterfaceStateDown::processEvent(NdnAddrInterfaceEventType event) {
    switch (event) {
        case NdnAddrInterfaceEventType::INTERFACE_UP: {
            interface->setState(NdnAddrInterfaceStateType::WAITING);
            NdnAddrInterface* intf = interface;
            // start sending hello
            IOC->getTimer()->startTimer("hello_timer_" + to_string(interface->getInterfaceID()), NDNADDR_HELLOINTERVAL * 1000, [intf](string) -> bool {
                intf->getProtocol()->getCronjobHandler()->sendingHelloMessageCronJob(intf->getInterfaceID());
                return true;
            });
        }
    }
}