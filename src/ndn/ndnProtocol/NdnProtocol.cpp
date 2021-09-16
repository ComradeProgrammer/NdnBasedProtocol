#include "NdnProtocol.h"
using namespace std;

NdnProtocol::NdnProtocol(shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
    deadNonceList = make_shared<DeadNonceList>(logger);
    pit = make_shared<Pit>();
    cs = make_shared<ContentStore>();

    nextHopStrategy = make_shared<NextHopStrategyBroadcastToEveryoneElse>();
    forwardDataStrategy = make_shared<ForwardDataStrategyDefault>();
}
void NdnProtocol::onIncomingPacket(int interfaceIndex, MacAddress sourceMac,
                                   std::shared_ptr<NdnPacket> packet) {
    if (packet->getPacketType() == TLV_INTEREST) {
        auto interest = dynamic_pointer_cast<NdnInterest>(packet);
        onIncomingInterest(interfaceIndex, sourceMac, interest);
    } else if (packet->getPacketType() == TLV_DATA) {
        auto data = dynamic_pointer_cast<NdnData>(packet);
        onIncomingData(interfaceIndex, sourceMac, data);
    }
}

void NdnProtocol::onIncomingInterest(int interfaceIndex, MacAddress sourceMac,
                                     std::shared_ptr<NdnInterest> interest) {
    logger->INFOF(
        "Entering NdnProtocol::onIncomingInterest, from interface %d, "
        "macaddress %s, packet %s",
        interfaceIndex, sourceMac.toString().c_str(),
        interest->toString().c_str());
    // 1. whether the Interest has exceeded its stored HopLimit
    auto hopLimitPair = interest->getHopLimit();
    if (hopLimitPair.first == false && hopLimitPair.second == 0) {
        logger->WARNING("packet dropped due to hopLimit exceeded");
        return;
    }
    // checks for name and /localhost scope [9] violations.
    if (interest->getName() == "" || interest->getName() == "/") {
        logger->WARNING("packet dropped due to invalid name");
        return;
    }
    vector<string> nameSplits = split(interest->getName(), "/");
    if (nameSplits[1] == "localhost") {
        // FIXME: check whether it is really a localhost nic
        // now we don't use localhost, so just refuse it
        logger->WARNING(
            "packet dropped due to name prefix /localhost, but not from a "
            "loopback Lo");
        return;
    }
    // 3.The Name and Nonce of the incoming Interest are checked against the
    // Dead Nonce List
    if (deadNonceList->isInDeadNonceList(interest->getName(),
                                         interest->getNonce())) {
        // nonce and name duplicated in dead nonce list
        onInterestLoop(interfaceIndex, sourceMac, interest);
        return;
    }
    // 4. If the Interest carries a forwarding hint, the procedure determines
    // whether the Interest has reached the producer region... we dont use
    // forwarding hint, so just ignore it

    // 5.The next step is looking up existing or creating a new PIT entry
    protocolLock.lock();
    shared_ptr<PitEntry> pitEntry = pit->getPitEntry(interest->getName());
    logger->INFOF("NdnProtocol::onIncomingInterest: pit entry content: %s",
                  pitEntry->toString().c_str());
    // 6.Before the incoming Interest is processed any further, its Nonce is
    // checked against the Nonces among PIT in-records.
    if (pitEntry->isLoopingInterest(interfaceIndex, interest->getNonce())) {
        // nonce and name duplicated in pit
        onInterestLoop(interfaceIndex, sourceMac, interest);
        protocolLock.unlock();
        return;
    }
    // 7. Next, the expiry timer on the PIT entry is cancelled
    shared_ptr<Timer> timer = Timer::GetTimer();
    timer->cancelTimer(pitEntry->getTimerName());
    // 8. If the Interest is not pending, the Interest is matched against the
    // Content Store
    if (!pitEntry->isPending() &&
        cs->contentHit(interest->getName()) != nullptr) {
        onContentStoreHit(interfaceIndex, sourceMac, interest);
        protocolLock.unlock();
        return;
    }
    // 9. If the Interest is not pending, the Interest is matched against the
    // Content Store
    onContentStoreMiss(interfaceIndex, sourceMac, interest);
    protocolLock.unlock();
}

void NdnProtocol::onInterestLoop(int interfaceIndex, MacAddress sourceMac,
                                 std::shared_ptr<NdnInterest> interest) {
    // FIXME: This pipeline sends a Nack with reason code Duplicate to the
    // Interest incoming face, but currently we just do nothing
    logger->WARNINGF(
        "Entering NdnProtocol::onInterestLoop, from interface %d, "
        "macaddress %s, packet %s",
        interfaceIndex, sourceMac.toString().c_str(),
        interest->toString().c_str());
}

void NdnProtocol::onContentStoreHit(int interfaceIndex, MacAddress sourceMac,
                                    std::shared_ptr<NdnInterest> interest) {
    // TODO: impelement this part once the CS is implemented
}

void NdnProtocol::onContentStoreMiss(int interfaceIndex, MacAddress sourceMac,
                                     std::shared_ptr<NdnInterest> interest) {
    shared_ptr<PitEntry> pitEntry = pit->getPitEntry(interest->getName());
    // 1.An in-record for the Interest and its incoming face is inserted into
    // the PIT entry.
    pitEntry->addInputRecord(interfaceIndex, interest->getNonce());
    // 2. The expiry timer on the PIT entry is set to the time that the last PIT
    // in-record expires
    shared_ptr<Timer> timer = Timer::GetTimer();
    timer->startTimer(
        pitEntry->getTimerName(), PIT_EXPIRE_DURATION,
        [this, interfaceIndex, sourceMac, interest](string) -> bool {
            protocolLock.lock();
            onInterestFinalize(interfaceIndex, sourceMac, interest);
            protocolLock.unlock();
            return false;
        });
    // 3.If the Interest carries a NextHopFaceId field in its NDNLPv2 header,the
    // pipeline honors this field but we don't use this field, so just override
    // this step.

    // 4 choose next hop faces;
    vector<pair<int, MacAddress>> faces =
        (*nextHopStrategy)(interfaceIndex, sourceMac, interest);
    onOutgoingInterest(interfaceIndex, sourceMac, interest, faces);
}

void NdnProtocol::onOutgoingInterest(int interfaceIndex, MacAddress sourceMac,
                                     std::shared_ptr<NdnInterest> interest,
                                     vector<pair<int, MacAddress>> faces) {
    logger->INFO(
        string("Entering NdnProtocol::onOutgoingInterest, target interfaces ") +
        intMacAddressVectorToString(faces));
    // 1. First, it is determined whether the Interest has exceeded its HopLimit
    auto hopLimitPair = interest->getHopLimit();
    if (hopLimitPair.first == false && hopLimitPair.second <= 1) {
        logger->WARNING("packet rejected to be sent due to hopLimit exceeded");
        return;
    }
    // 2.Next, an out-record is inserted into the PIT entry for the specified
    // outgoing Face. / we dont't have this mechanism

    // 3.Finally, the Interest is sent to the outgoing Face
    auto transmitter = NdnTransmitter::getTransmitter();
    // make a copy this packet.
    shared_ptr<NdnInterest> newInterest = make_shared<NdnInterest>(*interest);
    // TODO: Add support for upper layer protocol
    for (auto interfaceInfo : faces) {
        transmitter->send(interfaceInfo.first, interfaceInfo.second,
                          newInterest);
    }
}

void NdnProtocol::onInterestFinalize(int interfaceIndex, MacAddress sourceMac,
                                     std::shared_ptr<NdnInterest> interest) {
    shared_ptr<PitEntry> pitEntry = pit->findPitEntry(interest->getName());
    if (pitEntry == nullptr) {
        return;
    }
    for (auto nonce : pitEntry->getAllNonce()) {
        deadNonceList->addToDeadNonceList(interest->getName(), nonce);
    }
    pit->deletePitEntry(interest->getName());
}
void NdnProtocol::onIncomingData(int interfaceIndex, MacAddress sourceMac,
                                 std::shared_ptr<NdnData> data) {
    logger->INFOF(
        "NdnProtocol::onIncomingData, from interface %d, "
        "macaddress %s, packet %s",
        interfaceIndex, sourceMac.toString().c_str(), data->toString().c_str());

    // checks for name and /localhost scope [9] violations.
    if (data->getName() == "" || data->getName() == "/") {
        logger->WARNING("packet dropped due to invalid name");
        return;
    }
    vector<string> nameSplits = split(data->getName(), "/");
    if (nameSplits[1] == "localhost") {
        // FIXME: check whether it is really a localhost nic
        // now we don't use localhost, so just refuse it
        logger->WARNING(
            "packet dropped due to name prefix /localhost, but not from a "
            "loopback Lo");
        return;
    }
    // Then, the pipeline checks if the Data matches PIT entries,
    protocolLock.lock();
    auto pitEntry = pit->findPitEntry(data->getName());
    if (pitEntry == nullptr) {
        onDataUnsolicited(interfaceIndex, sourceMac, data);
        protocolLock.unlock();
        return;
    }
    // if matching PIT entries are found, the Data is inserted into the Content
    // Store
    // TODO: implement CS operation
    
    // will set the PIT expiry timer to now. for us, we just cancel the timer
    // and manually do the finalizing job
    Timer::GetTimer()->cancelTimer(pitEntry->getTimerName());
    for (auto nonce : pitEntry->getAllNonce()) {
        deadNonceList->addToDeadNonceList(data->getName(), nonce);
    }
    pit->deletePitEntry(data->getName());
    auto faces =
        (*forwardDataStrategy)(interfaceIndex, sourceMac, data, pitEntry);
    onOutgoingData(interfaceIndex, sourceMac, data, faces);
    protocolLock.unlock();
}
void NdnProtocol::onDataUnsolicited(int interfaceIndex, MacAddress sourceMac,
                                    std::shared_ptr<NdnData> data) {
    logger->INFOF(
        "NdnProtocol::onDataUnsolicited, from interface %d, "
        "macaddress %s, packet %s",
        interfaceIndex, sourceMac.toString().c_str(), data->toString().c_str());
}

void NdnProtocol::onOutgoingData(
    int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data,
    std::vector<std::pair<int, MacAddress>> faces) {
    logger->INFO(
        string("Entering NdnProtocol::onOutgoingData, target interfaces ") +
        intMacAddressVectorToString(faces));
    auto transmitter = NdnTransmitter::getTransmitter();
    // make a copy this packet.
    shared_ptr<NdnData> newData = make_shared<NdnData>(*data);
    // TODO: Add support for upper layer protocol
    for (auto interfaceInfo : faces) {
        transmitter->send(interfaceInfo.first, interfaceInfo.second, newData);
    }
}