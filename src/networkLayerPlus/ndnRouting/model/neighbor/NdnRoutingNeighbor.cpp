#include"NdnRoutingNeighbor.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"

using namespace std;
NdnRoutingNeighbor::NdnRoutingNeighbor(NdnRoutingInterface* _root):interface(_root){
    state = make_shared<NdnRoutingNeighborStateDown>(this);
}
void NdnRoutingNeighbor::processEvent(NeighborEventType e){
    LOGGER->INFOF("interface %d, neighbor %s(rid:%d) process Event %s, current state %s",
    interface->getInterfaceID(),ipv4Addr.toString().c_str(),routerID,
    getNameForNeighborEvent(e).c_str(),
    getNameForNeighborState(state->getState()).c_str()
    );
    state->processEvent(e);
}
void NdnRoutingNeighbor::setState(NeighborStateType stateType){
    LOGGER->INFOF("interface %d neighbor %s(rid %d) change to state %s from %s",
    interface->getInterfaceID(),ipv4Addr.toString().c_str(),routerID,
    getNameForNeighborState(state->getState()).c_str(),
    getNameForNeighborState(stateType).c_str());

    shared_ptr<NdnRoutingNeighborState> newState = nullptr;
    switch (stateType) {
        case NeighborStateType::DOWN:
            newState = make_shared<NdnRoutingNeighborStateDown>(this);
            break;
        case NeighborStateType::INIT:
            newState = make_shared<NdnRoutingNeighborStateInit>(this);
            break;
        case NeighborStateType::EXCHANGE:
            newState = make_shared<NdnRoutingNeighborStateExchange>(this);
            break;
        case NeighborStateType::LOADING:
            newState = make_shared<NdnRoutingNeighborStateLoading>(this);
            break;
        case NeighborStateType::FULL:
            newState = make_shared<NdnRoutingNeighborStateFull>(this);
            break;
    }
    state = newState;
}
void NdnRoutingNeighbor::recordTimer(string timerName){
    activeTimers.insert(timerName);
}
void NdnRoutingNeighbor::deleteTimer(string timerName){
    IOC->getTimer()->cancelTimer(timerName);
    activeTimers.erase(timerName);
}
void NdnRoutingNeighbor::clear(){
    for (auto timerName : activeTimers) {
        IOC->getTimer()->cancelTimer(timerName);
    }
    //todo implement
}