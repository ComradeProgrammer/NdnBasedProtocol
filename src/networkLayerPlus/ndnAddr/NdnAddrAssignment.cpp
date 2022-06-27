#include "NdnAddrAssignment.h"
using namespace std;

NdnAddrAssignmentProtocol::NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol)
    : routerID(_routerID), ndnProtocol(_ndnProtocol) {
    cronjobController = make_shared<AddrCronjobController>(this);
    helloController = make_shared<AddrHelloController>(this);
    mutexLock = make_shared<mutex>();
}

void NdnAddrAssignmentProtocol::onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    auto splits = split(packet->getName(), "/");
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            LOGGER->INFOF(2, "NdnaddrProtocol INTEREST received, content %s from interface %d", packet->toString().c_str(), interfaceIndex);
            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "hello") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
                return;
            }
        }
        case TLV_DATA: {
            LOGGER->INFOF(2, "NdnRoutingProtocol DATA received, content %s", packet->toString().c_str());
        }
    }
}

void NdnAddrAssignmentProtocol::start() {
    auto nics = IOC->getNicManager()->getAllNicsInMap();
    for (auto nic : nics) {
        int intfID = nic.first;
        // create interfaces
        auto interface = make_shared<NdnAddrInterface>(this);
        interface->setName(nic.second->getName());
        interface->setInterfaceID(nic.second->getInterfaceID());
        interface->setMacAddress(nic.second->getMacAddress());
        interface->setIpv4Address(nic.second->getIpv4Address());
        interface->setIpv4Mask(nic.second->getIpv4Mask());
        IOC->getNicManager()->registerObserver(interface.get(), interface->getInterfaceID());
        interfaces[interface->getInterfaceID()] = interface;
        interface->processInterfaceEvent(NdnAddrInterfaceEventType::INTERFACE_UP);
    }
}

void NdnAddrAssignmentProtocol::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    ndnProtocol->onReceiveNdnPacket(NDN_ADDRASSIGNMENT, sourceMac, packet);
    return;
}