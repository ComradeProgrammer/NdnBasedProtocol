#include "NdnProtocol.h"
using namespace std;
NdnProtocol::NdnProtocol(shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
}
void NdnProtocol::onReceiveInterest(int interfaceIndex, MacAddress sourceMac,
                                    std::shared_ptr<NdnInterest> interest) {
    logger->INFOF(
        "Entering NdnProtocol::onReceiveInterest, from interface %d, "
        "macaddress %s, packet %s",
        interfaceIndex, sourceMac.toString().c_str(),
        interest->toString().c_str());
    // 1. whether the Interest has exceeded its stored HopLimit
    auto hopLimitPair = interest->getHopLimit();
    if (hopLimitPair.first == true && hopLimitPair.second == 0) {
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
        // TODO: check whether it is really a localhost nic
        // now we don't use localhost, so just refuse it
        logger->WARNING(
            "packet dropped due to name prefix /localhost, but not from a "
            "loopback Lo");
        return;
    }
    //
}