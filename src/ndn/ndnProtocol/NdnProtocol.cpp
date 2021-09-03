#include "NdnProtocol.h"
using namespace std;
NdnProtocol::NdnProtocol(shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
    deadNonceList = make_shared<DeadNonceList>(logger);
    pit = make_shared<Pit>();
    cs = make_shared<ContentStore>();
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
        return;
    }
    // 9. If the Interest is not pending, the Interest is matched against the
    // Content Store
    onContentStoreMiss(interfaceIndex, sourceMac, interest);
}

void NdnProtocol::onInterestLoop(int interfaceIndex, MacAddress sourceMac,
                                 std::shared_ptr<NdnInterest> interest) {
    // FIXME: This pipeline sends a Nack with reason code Duplicate to the
    // Interest incoming face, but currently we just do nothing
    logger->WARNINGF(
        "Entering NdnProtocol::onIncomingInterest, from interface %d, "
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
    // choose next hop faces;
}

void NdnProtocol::onInterestFinalize(int interfaceIndex, MacAddress sourceMac,
                                     std::shared_ptr<NdnInterest> interest) {
    // TODO: IMPLEMENT
}