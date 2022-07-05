#include "NdnAddrNeighbor.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
#include "networkLayerPlus/ndnAddr/model/interface/NdnAddrInterface.h"
using namespace std;

string getNameForAddrNeighborEventType(NdnAddrNeighborEventType event) {
    switch (event) {
        case NdnAddrNeighborEventType::HELLO_RECEIVE:
            return "HELLO_RECEIVED";
        case NdnAddrNeighborEventType::KILL_NEIGHBOR:
            return "KILL_NEIGHBOR";
    }
    return "";
}
void NdnAddrNeighbor::processEvent(NdnAddrNeighborEventType event) {
    LOGGER->INFOF(3, "neighbor %d process event %s", routerID, getNameForAddrNeighborEventType(event).c_str());
    if (event == NdnAddrNeighborEventType::HELLO_RECEIVE) {
        isStateUp = true;
        string name = "inactive_timer" + to_string(interface->getInterfaceID()) + "_" + to_string(routerID);
        IOC->getTimer()->cancelTimer(name);
        IOC->getTimer()->startTimer(name, NDNADDR_ROUTERDEADINTERVAL * 1000, [this](string) -> bool {
            interface->getProtocol()->getCronjobHandler()->inactiveTimer(this);
            return false;
        });
    } else if (event == NdnAddrNeighborEventType::KILL_NEIGHBOR) {
        isStateUp = false;
    }
}