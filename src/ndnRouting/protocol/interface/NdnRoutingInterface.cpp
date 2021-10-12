#include "NdnRoutingInterface.h"

#include "ndnRouting/protocol/NdnRoutingProtocol.h"
using namespace std;
NdnRoutingInterface::NdnRoutingInterface(NIC nic,
                                         std::shared_ptr<Logger> _logger) {
    name = nic.getName();
    interfaceID = nic.getInterfaceID();
    macAddress = nic.getMacAddress();
    ipv4Addr = nic.getIpv4Address();
    ipv4Mask = nic.getIpv4Mask();

    logger = Logger::getDefaultLoggerIfNull(_logger);
    state = make_shared<NdnRoutingInterfaceStateDown>(this);
}

void NdnRoutingInterface::changeState(
    NdnRoutingInterfaceStateType newStateType) {
    shared_ptr<NdnRoutingInterfaceState> newState = nullptr;
    switch (newStateType) {
        case UP:
            newState = make_shared<NdnRoutingInterfaceStateUp>(this);
            break;
        case DOWN:
            newState = make_shared<NdnRoutingInterfaceStateDown>(this);
            break;
    }
    state = newState;
}

void NdnRoutingInterface::processStateEvent(
    NdnRoutingInterfaceEventType event) {
    state->processEvent(event);
}

void NdnRoutingInterface::sendHelloInterests() {
    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();
    // construct hello packets
    HelloInterestPack helloPack;
    helloPack.routerId =
        NdnRoutingProtocol::getNdnRoutingProtocol()->getRouterID();
    helloPack.interfaceIP = ipv4Addr;
    helloPack.networkMask = ipv4Mask;
    helloPack.helloInterval = NDNROUTING_HELLOINTERVAL;
    helloPack.routerDeadInterval = NDNROUTING_ROUTERDEADINTERVAL;
    // TODO: add neighbors into hello interests;

    auto encodePair = helloPack.encode();
    auto packet = make_shared<NdnInterest>(logger);
    packet->setName("/routing/local/hello");
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces(
        {{interfaceID, MacAddress("ff:ff:ff:ff:ff:ff")}});
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(macAddress, packet);

    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
}

void NdnRoutingInterface::clear(){
    
}
