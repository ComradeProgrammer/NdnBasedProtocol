#include "NdnRoutingInterface.h"
using namespace std;
NdnRoutingInterface::NdnRoutingInterface(NdnRoutingProtocol* _protocol) : protocol(_protocol) {
    state = make_shared<NdnRoutingInterfaceDown>(this);
}
void NdnRoutingInterface::onEventHappen(int interfaceID, NICEvent event) {}
void NdnRoutingInterface::setState(NdnRoutingInterfaceStateType newStateType) {
    LOGGER->INFOF("Interface State changed on interface %d , from %s to %s", interfaceID, getNameForInterfaceStateType(state->getState()).c_str(),
                  getNameForInterfaceStateType(newStateType).c_str());
    shared_ptr<NdnRoutingInterfaceState> newState = nullptr;
    switch (newStateType) {
        case NdnRoutingInterfaceStateType::UP:
            newState = make_shared<NdnRoutingInterfaceUp>(this);
            break;
        case NdnRoutingInterfaceStateType::DOWN:
            newState = make_shared<NdnRoutingInterfaceDown>(this);
            break;
    }
    state = newState;

}
NdnRoutingInterfaceStateType NdnRoutingInterface::getState() {
    return state->getState();
}

void NdnRoutingInterface::processInterfaceEvent(NdnRoutingInterfaceEventType event){
    LOGGER->INFOF("interface %d process event %s on state %s", interfaceID, getNameForInterfaceEventType(event).c_str(),getNameForInterfaceStateType(state->getState()).c_str());
    state->processEvent(event);
}
void NdnRoutingInterface::clear() {
    for (auto neighbor : neighbors) {
        neighbor.second->processEvent(NeighborEventType::LL_DOWN);
    }
    neighbors.clear();
}
