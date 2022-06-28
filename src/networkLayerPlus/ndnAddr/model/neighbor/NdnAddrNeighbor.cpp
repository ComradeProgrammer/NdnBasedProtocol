#include "NdnAddrNeighbor.h"
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
    isStateUp = true;
}