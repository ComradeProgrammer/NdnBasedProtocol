#include "NdnAddrInterface.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"

using namespace std;
NdnAddrInterface::NdnAddrInterface(NdnAddrAssignmentProtocol* _protocol) {
    protocol = _protocol;
    state = make_shared<NdnAddrInterfaceStateDown>(this);
}

void NdnAddrInterface::onEventHappen(int interfaceID, NICEvent event) {}

void NdnAddrInterface::setState(NdnAddrInterfaceStateType stateType) {
    LOGGER->INFOF(3, "Interface State changed on interface %d , from %s to %s", interfaceID, getNameForAddrInterfaceStateType(state->getState()).c_str(),
                  getNameForAddrInterfaceStateType(stateType).c_str());
    switch (stateType) {
        case NdnAddrInterfaceStateType::DOWN:
            state = make_shared<NdnAddrInterfaceStateDown>(this);
            break;
        case NdnAddrInterfaceStateType::WAITING:
            state = make_shared<NdnAddrInterfaceStateWaiting>(this);
            break;
        case NdnAddrInterfaceStateType::LEADER:
            state = make_shared<NdnAddrInterfaceStateLeader>(this);
            break;
        case NdnAddrInterfaceStateType::NORMAL:
            state = make_shared<NdnAddrInterfaceStateNormal>(this);
            break;
    }
}
void NdnAddrInterface::processInterfaceEvent(NdnAddrInterfaceEventType event) {
    LOGGER->INFOF(3, "interface %d process event %s on state %s", interfaceID, getNameForAddrInterfaceEventType(event).c_str(),
                  getNameForAddrInterfaceStateType(state->getState()).c_str());
    state->processEvent(event);
}

RouterID NdnAddrInterface::calculateLeader() {
    RouterID routerID = protocol->getRouterID();
    RouterID newLeader = leader;
    // leader=0 means no leader calculated
    // we don't need to calculate it if there has already been a leader
    if (newLeader == 0) {
        // no existing leader
        // check whether there has been a router declaring it is a leader
        RouterID biggestID = routerID;
        for (auto itr = neighbors.begin(); itr != neighbors.end(); itr++) {
            biggestID = max(biggestID, itr->second->getRouterID());
            RouterID neighborLeader = itr->second->getNeighborLeader();
            if (neighborLeader != 0) {
                leader = max(leader, newLeader);
            }
        }

        // if there is still no leader, assign router with highest routerID as leader
        if (newLeader == 0) {
            newLeader = biggestID;
        }
    }
    return newLeader;
}

std::shared_ptr<NdnAddrNeighbor> NdnAddrInterface::getNeighborByRouterID(RouterID id) {
    if (neighbors.find(id) != neighbors.end()) {
        return neighbors[id];
    }
    return nullptr;
}

void NdnAddrInterface::sendBroadcastAddrRequest() {
    int requestSize=neighbors.size()+1;
    string name="/addr/broadcast/req/"+to_string(protocol->getRouterID())+"/"+to_string(interfaceID)+"/"+to_string(requestSize);

    auto packet = make_shared<NdnInterest>();
    packet->setName(name);
    packet->setNonce(rand());
    LOGGER->INFOF(3,"NdnAddrInterface::sendBroadcastAddrRequest send out %s",name.c_str());

    protocol->sendPacket(macAddress,packet);
}
