#include "NdnRoutingProtocol.h"
using namespace std;
NdnRoutingProtocol::NdnRoutingProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol) : routerID(_routerID), ndnProtocol(_ndnProtocol) {
    database = make_shared<LsaDatabase>();

    cronJobHandler = make_shared<CronJobHandler>(this);
    helloController = make_shared<HelloController>(this);
    ddController = make_shared<DDController>(this);
    lsaController = make_shared<LsaController>(this);
    registerController = make_shared<RegisterController>(this);

    // test code
    // for (int i = 0; i < 5; i++) {
    //     shared_ptr<LsaDataPack> lsa = make_shared<LsaDataPack>();
    //     lsa->lsType = LinkStateType::RCH;
    //     lsa->routerID = i * 100 + routerID;
    //     lsa->seqNum = 5;
    //     lsa->lsAge = 5;
    //     lsa->numberOfLinks = 1;

    //     NdnLink tmp;
    //     tmp.linkType = LinkType::POINTTOPOINT_LINK;
    //     tmp.linkID = 1;
    //     tmp.linkData = 2;
    //     tmp.linkDataMask = 3;
    //     tmp.linkCost = 4;

    //     lsa->links.push_back(tmp);
    //     database->insertLsa(lsa);
    // }
}

void NdnRoutingProtocol::onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnPacket> packet) {
    auto splits = split(packet->getName(), "/");
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            LOGGER->INFOF(2, "NdnRoutingProtocol INTEREST received, content %s from interface %d", packet->toString().c_str(),interfaceIndex);

            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "hello") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "LSA") {
                lsaController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "INFO") {
                // onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "register") {
                registerController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
        case TLV_DATA: {
            LOGGER->INFOF(2, "NdnRoutingProtocol DATA received, content %s", packet->toString().c_str());

            auto data = dynamic_pointer_cast<NdnData>(packet);
            if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveData(interfaceIndex, sourceMac, data);
            } else if (splits.size() > 3 && splits[3] == "LSA") {
                lsaController->onReceiveData(interfaceIndex, sourceMac, data);
            } else if (splits.size() > 3 && splits[3] == "INFO") {
                // onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "register") {
                registerController->onReceiveData(interfaceIndex, sourceMac, data);
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

bool NdnRoutingProtocol::inBroadcastLsaPendingRequestList(LinkStateType lsaType, RouterID routerID, uint32_t sequenceNum) {
    for (auto digest : broadcastLsaPendingRequestList) {
        if (digest.linkStateType == lsaType && digest.routerID == routerID && digest.sequenceNum == sequenceNum) {
            return true;
        }
    }
    return false;
}

shared_ptr<LsaDataPack> NdnRoutingProtocol::generateLsa() {
    shared_ptr<LsaDataPack> lsa = make_shared<LsaDataPack>();
    lsa->routerID = routerID;
    lsa->seqNum = 0;
    lsa->lsAge = 0;
    lsa->numberOfLinks = 0;  // will be increased
    for (auto interfacePair : interfaces) {
        if (interfacePair.second->getState() == NdnRoutingInterfaceStateType::DOWN) {
            continue;
        }
        for (auto neighborPair : interfacePair.second->getNeighbors()) {
            NdnLink link;
            // todo: add more types here
            link.linkType = LinkType::POINTTOPOINT_LINK;
            link.linkID = neighborPair.second->getRouterID();
            link.linkData = neighborPair.second->getIpv4Address().addr;
            link.linkDataMask = neighborPair.second->getIpv4Mask().addr;
            link.linkCost = interfacePair.second->getCost();
            lsa->numberOfLinks++;
            lsa->links.push_back(link);
        }
    }

    // fill in the seqNum of new lsa
    auto existingLsa = database->findLsa(LinkStateType::ADJ, routerID);
    if (existingLsa != nullptr) {
        lsa->seqNum = existingLsa->seqNum + 1;
    }

    // insert new lsa and remove old one if necessary
    database->insertLsa(lsa);
    return lsa;
}
void NdnRoutingProtocol::registerParents() {
    // root->parent node
    auto res = database->calculateMinHopTree(routerID);

    // 1. find out all new parents we need to add/modify.
    for (auto pair : res) {
        RouterID root = pair.first;
        RouterID parent = pair.second;
        if (root == routerID) {
            continue;
        }
        if (registeredParents.find(root) != registeredParents.end() && registeredParents[root] != parent) {
            // send deregister packet
            sendDeregisterPacket(root, registeredParents[root]);
            registeredParents.erase(root);
        }

        if (registeredParents.find(root) == registeredParents.end()) {
            long timestamp = sendRegisterPacket(root, parent);
            registeredParents[root] = parent;
        }
    }
    for (auto pair : registeredParents) {
        RouterID root = pair.first;
        RouterID oldParent = pair.second;
        if (res.find(root) == res.end()) {
            sendDeregisterPacket(root, registeredParents[root]);
        }
    }
}

shared_ptr<NdnRoutingNeighbor> NdnRoutingProtocol::getNeighborByRouterID(RouterID id) {
    for (auto p : interfaces) {
        auto neighbor = p.second->getNeighborByRouterID(id);
        if (neighbor != nullptr) {
            return neighbor;
        }
    }
    return nullptr;
}

long NdnRoutingProtocol::sendRegisterPacket(RouterID root, RouterID parent) {
    LOGGER->INFOF(2, "sending RegisterInterest to %d for root %d", parent, root);
    auto neighbor = getNeighborByRouterID(parent);

    auto interfaceObj = interfaces[neighbor->getInterfaceID()];

    RegisterInterestPack registerPacket;
    registerPacket.root = root;
    registerPacket.linkStateType = LinkStateType::ADJ;
    auto lsa = database->findLsa(LinkStateType::ADJ, root);
    if (lsa == nullptr) {
        registerPacket.sequenceNum = -1;
    } else {
        registerPacket.sequenceNum = lsa->seqNum;
    }

    auto encodePair = registerPacket.encode();
    auto packet = make_shared<NdnInterest>();
    long timestamp = getTimeStamp();
    packet->setName("/routing/local/register/" + to_string(routerID) + "/" + to_string(timestamp));
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{neighbor->getInterfaceID(), neighbor->getMacAddress()}});
    unlock();
    sendPacket(interfaceObj->getMacAddress(), packet);
    lock();
    return timestamp;
}
long NdnRoutingProtocol::sendDeregisterPacket(RouterID root, RouterID parent) {
    LOGGER->INFOF(2, "sending DeregisterInterest to %d for root %d", parent, root);
}

long NdnRoutingProtocol::getLastRegistrationTime(RouterID root, RouterID son) {
    if (lastOperationTime.find(root) == lastOperationTime.end()) {
        lastOperationTime[root] = unordered_map<RouterID, long>();
    }
    if (lastOperationTime[root].find(son) == lastOperationTime[root].end()) {
        lastOperationTime[root][son] = 0;
    }
    return lastOperationTime[root][son];
}