#include "NdnAddrAssignment.h"
using namespace std;

NdnAddrAssignmentProtocol::NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol)
    : routerID(_routerID), ndnProtocol(_ndnProtocol) {}
void NdnAddrAssignmentProtocol::onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    LOGGER->WARNING("packet received");
}
void NdnAddrAssignmentProtocol::start() {
    auto nics = IOC->getNicManager()->getAllNicsInMap();
    for (auto nic : nics) {
        int intfID = nic.first;
        LOGGER->VERBOSEF("here %d", intfID);
        IOC->getTimer()->startTimer("hello_timer_" + to_string(intfID), 1 * 1000, [this](string) -> bool {
            auto tmp = make_shared<NdnInterest>();
            tmp->setName("/addr/lo/h");
            tmp->setNonce(rand());
            sendPacket(MacAddress("ff:ff:ff:ff:ff:ff"), tmp);
            return true;
        });
    }
}

void NdnAddrAssignmentProtocol::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    ndnProtocol->onReceiveNdnPacket(NDN_ADDRASSIGNMENT, sourceMac, packet);
    return;
}