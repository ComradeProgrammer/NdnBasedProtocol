#include "NdnRoutingNeighbor.h"
#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/dataPack/DDInterestPack.h"

using namespace std;
NdnRoutingNeighbor::NdnRoutingNeighbor(NdnRoutingInterface* _root) : interface(_root) { state = make_shared<NdnRoutingNeighborStateDown>(this); }
void NdnRoutingNeighbor::processEvent(NeighborEventType e) {
    LOGGER->INFOF("interface %d, neighbor %s(rid:%d) process Event %s, current state %s", interface->getInterfaceID(), ipv4Addr.toString().c_str(), routerID,
                  getNameForNeighborEvent(e).c_str(), getNameForNeighborState(state->getState()).c_str());
    state->processEvent(e);
}
void NdnRoutingNeighbor::setState(NeighborStateType stateType) {
    LOGGER->INFOF("interface %d neighbor %s(rid %d) change to state %s from %s", interface->getInterfaceID(), ipv4Addr.toString().c_str(), routerID,
                  getNameForNeighborState(state->getState()).c_str(), getNameForNeighborState(stateType).c_str());

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
void NdnRoutingNeighbor::recordTimer(string timerName) { activeTimers.insert(timerName); }
void NdnRoutingNeighbor::deleteTimer(string timerName) {
    IOC->getTimer()->cancelTimer(timerName);
    activeTimers.erase(timerName);
}
void NdnRoutingNeighbor::clear() {
    for (auto timerName : activeTimers) {
        IOC->getTimer()->cancelTimer(timerName);
    }
    // todo implement
}

void NdnRoutingNeighbor::createDatabaseSummary() {
    // choose the lsa which have a proper age
    auto database=interface->getProtocol()->getLsaDatabase();
    auto& adj=database->getAdjLsa();
    for (int i = 0; i < adj.size(); i++) {
        if (adj[i]->lsAge < NDN_ROUTING_MAX_AGE) {
            databaseSummary.push_back(adj[i]->generateLSDigest());
        }
    }

    auto& rch=database->getRchLsa();
    for (int i = 0; i < rch.size(); i++) {
        if (rch[i]->lsAge < NDN_ROUTING_MAX_AGE) {
            databaseSummary.push_back(rch[i]->generateLSDigest());
        }
    }
}
void NdnRoutingNeighbor::sendDDInterest(){
    string name = "/routing/local/dd/" + to_string(routerID) + "/" + to_string(recvingIndex);

    // construct a dd interest pack

    // DDInterestPack interestPack;
    // interestPack.neighbor = routerID;
    // auto encodePair = interestPack.encode();

    // construct a dd interest
    auto packet = make_shared<NdnInterest>();
    packet->setName(name);
    packet->setNonce(rand());
    //packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{interface->getInterfaceID(), macAddr}});

    //set up expired timer
    string timerName = string("dd_interest_timer") + to_string(interface->getInterfaceID()) + "_" +
                       to_string(routerID) + "_" + to_string(recvingIndex);
    // start the timer first then send packets, in case that the response goes back so quickly that timer hasn't been

    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;

    IOC->getTimer()->startTimer(
        timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000,
        [this, packet, retransmissionTime](string name)->bool{
            return interface->getProtocol()->getCrobJobHandler()->ddInterestExpireCronJob(retransmissionTime,packet,interface->getMacAddress(),name);
        }
    );

    interface->getProtocol()->unlock();
    interface->getProtocol()->sendPacket(interface->getMacAddress(),packet);
    interface->getProtocol()->lock();
}
