#include "NdnRoutingNeighborState.h"
#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"
using namespace std;

void NdnRoutingNeighborState::triggerNewLsa() {
    auto lsa = neighbor->getBelongingInterface()->getProtocol()->generateLsa();
    neighbor->getBelongingInterface()->getProtocol()->registerParents();
    neighbor->getBelongingInterface()->getProtocol()->sendInfoToChildren(lsa);
    neighbor->getBelongingInterface()->getProtocol()->rebuildRoutingTable();
}

string getNameForNeighborEvent(NeighborEventType eventType) {
    switch (eventType) {
        case NeighborEventType::HELLO_RECEIVED:
            return "HELLO_RECEIVED";
            break;
        case NeighborEventType::TWOWAY_RECEIVED:
            return "TWOWAY_RECEIVED";
            break;
        case NeighborEventType::ONEWAY_RECEIVED:
            return "ONEWAY_RECEIVED";
            break;
        case NeighborEventType::KILL_NEIGHBOR:
            return "KILL_NEIGHBOR";
            break;
        case NeighborEventType::INACTIVITY_TIMER:
            return "INACTIVITY_TIMER";
            break;
        case NeighborEventType::LL_DOWN:
            return "LL_DOWN";
            break;
        case NeighborEventType::EXCHANGE_DONE:
            return "EXCHANGE_DONE";
            break;
        case NeighborEventType::LOADING_DONE:
            return "LOADING_DONE";
            break;
        case NeighborEventType::INVALID_HASH:
            return "INVALID_HASH";
            break;
    }
    return "";
}
string getNameForNeighborState(NeighborStateType stateType) {
    switch (stateType) {
        case NeighborStateType::DOWN:
            return "DOWN_STATE";
            break;
        case NeighborStateType::INIT:
            return "INIT_STATE";
            break;
        case NeighborStateType::EXCHANGE:
            return "EXCHANGE_STATE";
            break;
        case NeighborStateType::LOADING:
            return "LOADING_STATE";
            break;
        case NeighborStateType::FULL:
            return "FULL_STATE";
            break;
    }
    return "";
}