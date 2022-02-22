#include "NdnRoutingProtocol.h"
using namespace std;
NdnRoutingProtocol::NdnRoutingProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol) : routerID(_routerID), ndnProtocol(_ndnProtocol) {
    database = make_shared<LsaDatabase>();

    cronJobHandler = make_shared<CronJobHandler>(this);
    helloController = make_shared<HelloController>(this);
    ddController = make_shared<DDController>(this);

    // test code
    for (int i = 0; i < 5; i++) {
        shared_ptr<LsaDataPack> lsa = make_shared<LsaDataPack>();
        lsa->lsType = LinkStateType::ADJ;
        lsa->routerID = 100 + i;
        lsa->seqNum = 5;
        lsa->lsAge = 5;
        lsa->numberOfLinks = 1;

        NdnLink tmp;
        tmp.linkType = LinkType::POINTTOPOINT_LINK;
        tmp.linkID = 1;
        tmp.linkData = 2;
        tmp.linkDataMask = 3;
        tmp.linkCost = 4;

        lsa->links.push_back(tmp);
        database->insertLsa(lsa);
    }
}

void NdnRoutingProtocol::onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnPacket> packet) {
    LOGGER->INFOF("NdnRoutingProtocol::onReceiveNdnPacket got %s", packet->getName().c_str());
    auto splits = split(packet->getName(), "/");
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "hello") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveInterest(interfaceIndex, sourceMac, interest);
                // onReceiveDDInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "LSA") {
                // onReceiveLsaInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "INFO") {
                // onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
        case TLV_DATA: {
            auto data = dynamic_pointer_cast<NdnData>(packet);
            if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveData(interfaceIndex, sourceMac, data);
            } else if (splits.size() > 3 && splits[3] == "LSA") {
                // onReceiveLsaInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "INFO") {
                // onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
    }
}

void NdnRoutingProtocol::start() {
    mutexLock = make_shared<mutex>();
    // establish interface structure
    auto nics = IOC->getNicManager()->getAllNicsInMap();
    for (auto nic : nics) {
        auto interface = make_shared<NdnRoutingInterface>(this);
        interface->setName(nic.second->getName());
        interface->setInterfaceID(nic.second->getInterfaceID());
        interface->setMacAddress(nic.second->getMacAddress());
        interface->setIpv4Address(nic.second->getIpv4Address());
        interface->setIpv4Mask(nic.second->getIpv4Mask());
        IOC->getNicManager()->registerObserver(interface.get(), interface->getInterfaceID());
        interfaces[interface->getInterfaceID()] = interface;
        interface->processInterfaceEvent(NdnRoutingInterfaceEventType::INTERFACE_UP);
    }
}

void NdnRoutingProtocol::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    ndnProtocol->onReceiveNdnPacket(NDN_ROUTING, sourceMac, packet);
    return;
}