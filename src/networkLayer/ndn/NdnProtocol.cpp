#include "NdnProtocol.h"

#include "util/traceback/traceback.h"
using namespace std;

NdnProtocol::NdnProtocol() {
    deadNonceList = make_shared<DeadNonceList>();
    pit = make_shared<Pit>();
    cs = make_shared<ContentStore>();
    nextHopStrategy = make_shared<MyNextHopStrategy>();
    forwardDataStrategy = make_shared<MyForwardStrategy>();
}

void NdnProtocol::onReceiveEthernetPacket(int sourceInterface, shared_ptr<EthernetPacket> ethPacket) {
    auto packet = NdnPacket::decode(ethPacket->getData());
    onReceiveNdnPacket(sourceInterface, ethPacket->getHeader().getSourceMacAddress(), packet);
}
void NdnProtocol::onReceiveNdnPacket(int sourceInterface, MacAddress sourceMac, shared_ptr<NdnPacket> packet) {
    if (packet->getPacketType() == TLV_INTEREST) {
        auto interest = dynamic_pointer_cast<NdnInterest>(packet);
        onIncomingInterest(sourceInterface, sourceMac, interest);
    } else if (packet->getPacketType() == TLV_DATA) {
        auto data = dynamic_pointer_cast<NdnData>(packet);
        onIncomingData(sourceInterface, sourceMac, data);
    }
}

void NdnProtocol::registerUpperLayerProtocol(int interfaceID, NdnProtocolPlus* protocol) {
    protocolLock.lock();
    upperLayerProtocols[interfaceID] = protocol;
    protocolLock.unlock();
}

void NdnProtocol::onIncomingInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    LOGGER->INFOF(1,
                  "Entering NdnProtocol::onIncomingInterest, from interface %d, "
                  "macaddress %s, packet %s ",
                  interfaceIndex, sourceMac.toString().c_str(), interest->toString().c_str());
    // 1. whether the Interest has exceeded its stored HopLimit
    auto hopLimitPair = interest->getHopLimit();
    if (hopLimitPair.first == false && hopLimitPair.second == 0) {
        LOGGER->WARNINGF("packet %s dropped due to hopLimit exceeded", interest->getName().c_str());
        return;
    }
    // 3.The Name and Nonce of the incoming Interest are checked against the
    // Dead Nonce List
    if (deadNonceList->isInDeadNonceList(interest->getName(), interest->getNonce())) {
        // nonce and name duplicated in dead nonce list
        onInterestLoop(interfaceIndex, sourceMac, interest);
        return;
    }
    // 5.The next step is looking up existing or creating a new PIT entry
    protocolLock.lock();
    if (!excludedFromPit(interest)) {
        shared_ptr<PitEntry> pitEntry = pit->getPitEntry(interest->getName());
        LOGGER->INFOF(1, "NdnProtocol::onIncomingInterest: packet %s pit entry content: %s", interest->getName().c_str(), pitEntry->toString().c_str());

        // 6.Before the incoming Interest is processed any further, its Nonce is
        // checked against the Nonces among PIT in-records.
        if (pitEntry->isLoopingInterest(interfaceIndex, interest->getNonce())/*temporaray code for hop lsa**/&&interfaceIndex>0) {
            // nonce and name duplicated in pit
            onInterestLoop(interfaceIndex, sourceMac, interest);
            protocolLock.unlock();
            return;
        }
        // 7. Next, the expiry timer on the PIT entry is cancelled
        shared_ptr<Timer> timer = IOC->getTimer();
        if (pitEntry->isPending()) {
            // no need to delete timer when the pit is new
            timer->cancelTimer(pitEntry->getTimerName());
        }
        // 8. If the Interest is not pending, the Interest is matched against the
        // Content Store
        if (!pitEntry->isPending() && cs->contentHit(interest->getName()) != nullptr) {
            onContentStoreHit(interfaceIndex, sourceMac, interest);
            protocolLock.unlock();
            return;
        }
    }
    onContentStoreMiss(interfaceIndex, sourceMac, interest);

    protocolLock.unlock();
}

void NdnProtocol::onInterestLoop(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    // This pipeline sends a Nack with reason code Duplicate to the
    // Interest incoming face, but currently we just do nothing
    LOGGER->WARNINGF(
        "Entering NdnProtocol::onInterestLoop, from interface %d, "
        "macaddress %s, packet %s",
        interfaceIndex, sourceMac.toString().c_str(), interest->toString().c_str());
}

void NdnProtocol::onContentStoreHit(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    // TODO: impelement this part once the CS is implemented
}

void NdnProtocol::onContentStoreMiss(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    bool isPending = false;
    if (!excludedFromPit(interest)) {
        shared_ptr<PitEntry> pitEntry = pit->getPitEntry(interest->getName());
        isPending = pitEntry->isPending();
        // 1.An in-record for the Interest and its incoming face is inserted into
        // the PIT entry.
        pitEntry->addInputRecord(interfaceIndex, interest->getNonce());
        // 2. The expiry timer on the PIT entry is set to the time that the last PIT
        // in-record expires
        shared_ptr<Timer> timer = IOC->getTimer();
        timer->startTimer(pitEntry->getTimerName(), PIT_EXPIRE_DURATION, [this, interfaceIndex, sourceMac, interest](string) -> bool {
            protocolLock.lock();
            onInterestFinalize(interfaceIndex, sourceMac, interest);
            protocolLock.unlock();
            return false;
        });
    }
    //temporaray code for hop lsa
    if(interfaceIndex<0){
        isPending=false;
    }
    if (isPending) {
        LOGGER->INFOF(1,
                      "NdnProtocol::onContentStoreMiss: no need to send interest for a "
                      "pending interest %s",
                      interest->toString().c_str());
        return;
    }
    vector<pair<int, MacAddress>> faces;
    if (interest->hasPreferedInterfaces()) {
        faces = interest->getPreferedInterfaces();
    } else {
        faces = (*nextHopStrategy)(interfaceIndex, sourceMac, interest);
    }
    onOutgoingInterest(interfaceIndex, sourceMac, interest, faces);
}

void NdnProtocol::onOutgoingInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest, vector<pair<int, MacAddress>> faces) {
    LOGGER->INFO(
        1, string("Entering NdnProtocol::onOutgoingInterest, target interfaces ") + intMacAddressVectorToString(faces) + " packet name " + interest->getName().c_str());
    // 1. First, it is determined whether the Interest has exceeded its HopLimit
    auto hopLimitPair = interest->getHopLimit();
    if (hopLimitPair.first == false && hopLimitPair.second <= 1) {
        LOGGER->WARNING("packet rejected to be sent due to hopLimit exceeded");
        return;
    }
    // 3.Finally, the Interest is sent to the outgoing Face
    // make a copy this packet.
    protocolLock.unlock();
    shared_ptr<NdnInterest> newInterest = make_shared<NdnInterest>(*interest);
    for (auto interfaceInfo : faces) {
        // sendPacket method may get jammed, or require the lock, so release the
        // lock
        sendPacket(interfaceInfo.first, interfaceInfo.second, newInterest, interfaceIndex, sourceMac);
    }
    protocolLock.lock();
}

void NdnProtocol::onInterestFinalize(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    shared_ptr<PitEntry> pitEntry = pit->findPitEntry(interest->getName());
    if (pitEntry == nullptr) {
        return;
    }
    for (auto nonce : pitEntry->getAllNonce()) {
        deadNonceList->addToDeadNonceList(interest->getName(), nonce);
    }
    pit->deletePitEntry(interest->getName());
}

void NdnProtocol::onIncomingData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data) {
    LOGGER->INFOF(1,
                  "NdnProtocol::onIncomingData, from interface %d, "
                  "macaddress %s, packet %s",
                  interfaceIndex, sourceMac.toString().c_str(), data->toString().c_str());
    if (data->getName() == "" || data->getName() == "/") {
        LOGGER->WARNING("packet dropped due to invalid name");
        return;
    }
    // Then, the pipeline checks if the Data matches PIT entries,
    protocolLock.lock();
    auto pitEntry = pit->findPitEntry(data->getName());
    if (!excludedFromPit(data)) {
        if (pitEntry == nullptr) {
            onDataUnsolicited(interfaceIndex, sourceMac, data);
            protocolLock.unlock();
            return;
        }
        for (auto nonce : pitEntry->getAllNonce()) {
            deadNonceList->addToDeadNonceList(data->getName(), nonce);
        }
        // will set the PIT expiry timer to now. for us, we just cancel the timer
        // and manually do the finalizing job
        pit->deletePitEntry(data->getName());
        IOC->getTimer()->cancelTimer(pitEntry->getTimerName());
    }
    // if matching PIT entries are found, the Data is inserted into the Content
    // Store
    if (!excludedFromPit(data)) {
        cs->insertPacket(data);
    }

    vector<pair<int, MacAddress>> faces;
    if (data->hasPreferedInterfaces()) {
        faces = data->getPreferedInterfaces();
    } else {
        faces = (*forwardDataStrategy)(interfaceIndex, sourceMac, data, pitEntry);
    }
    onOutgoingData(interfaceIndex, sourceMac, data, faces);
    protocolLock.unlock();
}

void NdnProtocol::onOutgoingData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data, std::vector<std::pair<int, MacAddress>> faces) {
    LOGGER->INFO(1, string("Entering NdnProtocol::onOutgoingData, target interfaces ") + intMacAddressVectorToString(faces));
    // make a copy this packet.
    protocolLock.unlock();
    shared_ptr<NdnData> newData = make_shared<NdnData>(*data);
    for (auto interfaceInfo : faces) {
        // sendPacket method may get jammed, or require the lock, so release the
        // lock
        sendPacket(interfaceInfo.first, interfaceInfo.second, newData, interfaceIndex, sourceMac);
    }
    protocolLock.lock();
}
void NdnProtocol::onDataUnsolicited(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data) {
    LOGGER->INFOF(1,
                  "NdnProtocol::onDataUnsolicited, from interface %d, "
                  "macaddress %s, packet %s",
                  interfaceIndex, sourceMac.toString().c_str(), data->toString().c_str());
}

void NdnProtocol::sendPacket(int targetInterfaceIndex, MacAddress destination, std::shared_ptr<NdnPacket> packet, int sourceInterfaceIndex,
                             MacAddress sourceMac) {
    if (targetInterfaceIndex < 0) {
        if (upperLayerProtocols.find(targetInterfaceIndex) == upperLayerProtocols.end()) {
            LOGGER->ERRORF("NdnProtocol::sendPacket unrecognized interface %d, packet %s", targetInterfaceIndex, packet->toString().c_str());
            return;
        }

        upperLayerProtocols[targetInterfaceIndex]->onReceiveNdnPacket(sourceInterfaceIndex, sourceMac, packet);
    } else {
        auto transmitter = IOC->getTransmitter();
        // when sending packet to other routers, source address should be replaced to ours
        auto nicMap = IOC->getNicManager()->getAllNicsInMap();
        MacAddress source = nicMap[targetInterfaceIndex]->getMacAddress();

        auto rawNdnPacket = packet->encode();
        auto ethernetPacket = make_shared<EthernetPacket>(destination, source, NDN_PROTOCOL, rawNdnPacket.second.get(), rawNdnPacket.first);

        int res = transmitter->sendPacket(targetInterfaceIndex, ethernetPacket);
        if (res < 0) {
            LOGGER->ERROR("NdnTransmitter::send: sendout packet to " + destination.toString() + " but return value " + to_string(res));
        }
    }
}

bool NdnProtocol::excludedFromPit(std::shared_ptr<NdnPacket> interest) {
    auto splits = split(interest->getName(), "/");
    if (splits.size() > 3 && (splits[1] == "routing" || splits[1] == "addr") && splits[2] == "local") {
        return true;
    }
    if (splits.size() > 3 && (splits[1] == "routing") && splits[3] == "INFO") {
        return true;
    }
    // if(splits.size()>3 && splits[1]=="routing" &&splits[3]=="hop"){
    //     return true;
    // }
    return false;
}