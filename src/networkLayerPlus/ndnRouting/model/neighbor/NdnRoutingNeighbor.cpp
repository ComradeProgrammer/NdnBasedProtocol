#include "NdnRoutingNeighbor.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"

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
    databaseSummary.clear();
    ddList.clear();

    localLsaPendingRequestList.clear();
}

void NdnRoutingNeighbor::createDatabaseSummary() {
    // choose the lsa which have a proper age
    auto database = interface->getProtocol()->getLsaDatabase();
    auto& adj = database->getAdjLsa();
    for (int i = 0; i < adj.size(); i++) {
        if (adj[i]->lsAge < NDN_ROUTING_MAX_AGE) {
            databaseSummary.push_back(adj[i]->generateLSDigest());
            LOGGER->VERBOSEF("createDatabaseSummary %s",adj[i]->toString().c_str());
        }
    }

    auto& rch = database->getRchLsa();
    for (int i = 0; i < rch.size(); i++) {
        if (rch[i]->lsAge < NDN_ROUTING_MAX_AGE) {
            databaseSummary.push_back(rch[i]->generateLSDigest());
        }
    }
}
void NdnRoutingNeighbor::sendDDInterest() {
    string name = "/routing/local/dd/" + to_string(routerID) + "/" + to_string(receivingIndex);

    // construct a dd interest pack

    // DDInterestPack interestPack;
    // interestPack.neighbor = routerID;
    // auto encodePair = interestPack.encode();

    // construct a dd interest
    auto packet = make_shared<NdnInterest>();
    packet->setName(name);
    packet->setNonce(rand());
    // packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{interface->getInterfaceID(), macAddr}});

    // set up expired timer
    string timerName = string("dd_interest_timer") + to_string(interface->getInterfaceID()) + "_" + to_string(routerID) + "_" + to_string(receivingIndex);
    // start the timer first then send packets, in case that the response goes back so quickly that timer hasn't been

    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;

    IOC->getTimer()->startTimer(timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000, [this, packet, retransmissionTime](string name) -> bool {
        return interface->getProtocol()->getCrobJobHandler()->ddInterestExpireCronJob(retransmissionTime, packet, interface->getMacAddress(), name);
    });
    //LOGGER->INFO("timer name " + timerName);
    recordTimer(timerName);

    interface->getProtocol()->unlock();
    interface->getProtocol()->sendPacket(interface->getMacAddress(), packet);
    interface->getProtocol()->lock();
}

bool NdnRoutingNeighbor::sendDDData(int requestedIndex, string name) {
    // check whether requestIndex is legal
    //  the requested index is not allowed to be smaller than recorded index-1 or even greater
    if ((requestedIndex == sendingIndex || requestedIndex == sendingIndex - 1) && (requestedIndex < databaseSummary.size() || requestedIndex == 0)) {
        auto packet = make_shared<NdnData>();
        packet->setName(name);
        if (requestedIndex == 0) {
            // we need to construct the transmitting list first
            // take fragment mechanism into consideration
            // DATA-TYPE(9) TLV-LENGTH(9) Name(9+9+namesize) Content(9+9+contentsize)
            // content size=8+12*num of digest
            int reservedLength = 9 + 9 + 9 + 9 + 9 + 9 + 8 + name.size() + 1;
            int remainingLength = MTU - reservedLength;
            int numberofDigest = remainingLength / 12; 

            // test code
            numberofDigest = 1;
            for (int i = 0; i < databaseSummary.size(); i++) {
                if (i % numberofDigest == 0) {
                    DDDataPack dataPack;
                    dataPack.neighbor = interface->getInterfaceID();
                    dataPack.interfaceMTU = MTU;
                    dataPack.numberOfDDPackets = databaseSummary.size() / numberofDigest;
                    ddList.push_back(dataPack);
                }
                ddList[i / numberofDigest].ls.push_back(databaseSummary[i]);
            }
            // even if the database Summary size is 0, we still need to send out a packet to inform that we have no digests
            if (databaseSummary.size() == 0) {
                DDDataPack dataPack;
                dataPack.neighbor = interface->getInterfaceID();
                dataPack.interfaceMTU = MTU;
                dataPack.numberOfDDPackets = 0;
                ddList.push_back(dataPack);
            }
        }
        // send out the dd data of given index
        auto encodedPair = ddList[requestedIndex].encode();
        packet->setContent(encodedPair.first, encodedPair.second.get());
        packet->setPreferedInterfaces({{interface->getInterfaceID(), macAddr}});

        interface->getProtocol()->unlock();
        interface->getProtocol()->sendPacket(interface->getMacAddress(), packet);
        interface->getProtocol()->lock();

        sendingIndex = requestedIndex + 1;
    } else {
        LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveDDInterset invalid index in name %s, currenting sending index %d", name.c_str(), sendingIndex);
        return false;
    }
    return true;
}

void NdnRoutingNeighbor::dragPeerToInit() {
    HelloInterestPack helloPack;
    helloPack.routerId = interface->getProtocol()->getRouterID();
    helloPack.interfaceIP = interface->getIpv4Address();
    helloPack.networkMask = interface->getIpv4Mask();
    helloPack.helloInterval = NDNROUTING_HELLOINTERVAL;
    helloPack.routerDeadInterval = NDNROUTING_ROUTERDEADINTERVAL;
    // no neighbor shouw: enough to drag peer to init by triggering 1-way
    auto encodePair = helloPack.encode();
    auto packet = make_shared<NdnInterest>();
    packet->setName("/routing/local/hello");
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{interface->getInterfaceID(), MacAddress("ff:ff:ff:ff:ff:ff")}});
    // change to 1-way
    clear();
    processEvent(NeighborEventType::ONEWAY_RECEIVED);
    interface->getProtocol()->unlock();
    interface->getProtocol()->sendPacket(interface->getMacAddress(), packet);
    interface->getProtocol()->lock();
}

void NdnRoutingNeighbor::sendLocalLsaInterest(LinkStateDigest digest) {
    localLsaPendingRequestList.push_back(digest);

    string name =
        "/routing/local/LSA/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string(digest.routerID) + "/" + to_string(digest.sequenceNum);
    LsaInterestPack lsaInterestPack;
    lsaInterestPack.routerID = digest.routerID;
    lsaInterestPack.sequenceNum = digest.sequenceNum;
    lsaInterestPack.lsType = digest.linkStateType;
    LOGGER->VERBOSEF("sendLocalLsaInterest %d",lsaInterestPack.routerID );

    auto encodePair = lsaInterestPack.encode();
    auto packet = make_shared<NdnInterest>();
    packet->setName(name);
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());

    // start retransmission timer
    string timerName = string("lsa_interest_timer") + to_string(interface->getInterfaceID()) + "_" + to_string(routerID) + "_" + to_string(digest.linkStateType) +
                  "_" + to_string(digest.sequenceNum);
    recordTimer(timerName);
    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;
    IOC->getTimer()->startTimer(timerName,NDNROUTING_DDRETRANSMISSION_INTERVAL*1000,[this, packet, retransmissionTime](string name) -> bool {
        return interface->getProtocol()->getCrobJobHandler()->localLsaExpireCronJob(retransmissionTime, packet, interface->getMacAddress(), name);
    });

    LOGGER->VERBOSEF("NdnRoutingNeighbor::sendLocalLsaInterest %s",packet->toString().c_str());
    interface->getProtocol()->unlock();
    interface->getProtocol()->sendPacket(interface->getMacAddress(), packet);
    interface->getProtocol()->lock();
}

void NdnRoutingNeighbor::cancelLsaInterestRequest(LinkStateDigest digest) {
    string timerName = string("lsa_interest_timer") + to_string(interface->getInterfaceID()) + "_" + to_string(routerID) + "_" +
                       to_string(digest.linkStateType) + "_" + to_string(digest.sequenceNum);
    deleteTimer(timerName);      

    for (auto itr = localLsaPendingRequestList.begin(); itr != localLsaPendingRequestList.end(); itr++) {
        if (itr->routerID == digest.routerID&&itr->linkStateType==digest.linkStateType && !(digest < (*itr))){
            LOGGER->INFOF(
                "NdnRoutingNeighbor::cancelLsaInterestRequest: digest removed from interface %d neighbor %d, digest %s",
                interface->getInterfaceID(), routerID, digest.toString().c_str());
            localLsaPendingRequestList.erase(itr);
            break;
        }
    }  
}

