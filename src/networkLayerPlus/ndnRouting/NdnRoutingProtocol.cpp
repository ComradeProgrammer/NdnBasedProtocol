#include "NdnRoutingProtocol.h"
using namespace std;
NdnRoutingProtocol::NdnRoutingProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol) : routerID(_routerID), ndnProtocol(_ndnProtocol) {
    database = make_shared<LsaDatabase>();
    minimumHopTree = make_shared<MinimumHopTree>();

    cronJobHandler = make_shared<CronJobHandler>(this);
    helloController = make_shared<HelloController>(this);
    ddController = make_shared<DDController>(this);
    lsaController = make_shared<LsaController>(this);
    registerController = make_shared<RegisterController>(this);
    deRegisterController = make_shared<DeRegisterController>(this);
    infoController = make_shared<InfoController>(this);
    initTime=getTimeStamp();

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
            LOGGER->INFOF(2, "NdnRoutingProtocol INTEREST received, content %s from interface %d", packet->toString().c_str(), interfaceIndex);

            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "h") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "LS") {
                lsaController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "I") {
                infoController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "re") {
                registerController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "de") {
                deRegisterController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
        case TLV_DATA: {
            LOGGER->INFOF(2, "NdnRoutingProtocol DATA received, content %s", packet->toString().c_str());

            auto data = dynamic_pointer_cast<NdnData>(packet);
            if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveData(interfaceIndex, sourceMac, data);
            } else if (splits.size() > 3 && splits[3] == "LS") {
                lsaController->onReceiveData(interfaceIndex, sourceMac, data);
            } else if (splits.size() > 3 && splits[3] == "I") {
                // onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "re") {
                registerController->onReceiveData(interfaceIndex, sourceMac, data);
            } else if (splits.size() > 3 && splits[3] == "de") {
                deRegisterController->onReceiveData(interfaceIndex, sourceMac, data);
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

void NdnRoutingProtocol::removeFromBroadcastLsaPendingRequestList(LinkStateType lsaType, RouterID routerID, uint32_t sequenceNum) {
    for (auto itr = broadcastLsaPendingRequestList.begin(); itr != broadcastLsaPendingRequestList.end(); itr++) {
        if (itr->linkStateType == lsaType && itr->routerID == routerID && itr->sequenceNum == sequenceNum) {
            broadcastLsaPendingRequestList.erase(itr);
            return;
        }
    }
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

    // LOGGER->VERBOSEF("insert %s",lsa->toString().c_str());
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
        auto registeredParent = minimumHopTree->getRegisteredParent(root);
        if (registeredParent.first && registeredParent.second != parent) {
            // if we have a previous different parent send deregister packet
            sendDeregisterPacket(root, registeredParent.second);
        }
        
        if (!(registeredParent.first && registeredParent.second == parent)) {
            long timestamp = sendRegisterPacket(root, parent);
            minimumHopTree->setRegisteredParent(root, parent);
        }
    }

    // remove those old parents which are no longer parents now
    for (auto pair : minimumHopTree->getRegisteredParents()) {
        RouterID root = pair.first;
        RouterID oldParent = pair.second;
        if (res.find(root) == res.end()) {
            sendDeregisterPacket(root, oldParent);
            minimumHopTree->removeRegisteredParent(root);
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
    long timestamp = getTimeStamp();

    auto neighbor = getNeighborByRouterID(parent);
    if (neighbor == nullptr) {
        LOGGER->WARNINGF("NdnRoutingProtocol::sendRegisterPacket parnet %d not found", parent);
        return timestamp;
    }
    auto interfaceObj = interfaces[neighbor->getInterfaceID()];

    RegisterInterestPack registerPacket;
    //registerPacket.root = root;
    auto lsa = database->findLsa(LinkStateType::ADJ, root);
    if (lsa == nullptr) {
        registerPacket.adjSequenceNum = -1;
    } else {
        registerPacket.adjSequenceNum = lsa->seqNum;
    }
    registerPacket.timestamp=timestamp;

    auto encodePair = registerPacket.encode();
    auto packet = make_shared<NdnInterest>();

    packet->setName("/rt/lo/re/" + to_string((unsigned long long)routerID) + "/" + to_string((unsigned long long)parent)+"/"+to_string((unsigned long long)root));
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{neighbor->getInterfaceID(), neighbor->getMacAddress()}});
    LOGGER->INFOF(2, "sending RegisterInterest %s to %d for root %d,content %s", packet->getName().c_str(), parent, root,
                  registerPacket.toString().c_str());

    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;
    string timerName = "register_" + packet->getName();
    IOC->getTimer()->startTimer(timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000, [this, packet, retransmissionTime, interfaceObj](string name) -> bool {
        return getCrobJobHandler()->registerExpireCronJob(retransmissionTime, packet, interfaceObj->getMacAddress(), name);
    });

    // unlock();
    sendPacket(interfaceObj->getMacAddress(), packet);
    // lock();
    return timestamp;
}

long NdnRoutingProtocol::sendDeregisterPacket(RouterID root, RouterID parent) {
    LOGGER->INFOF(2, "sending DeregisterInterest to %d for root %d", parent, root);
    long timestamp = getTimeStamp();
    auto neighbor = getNeighborByRouterID(parent);
    if (neighbor == nullptr) {
        LOGGER->WARNINGF("NdnRoutingProtocol::sendRegisterPacket parnet %d not found", parent);
        return timestamp;
    }
    auto interfaceObj = interfaces[neighbor->getInterfaceID()];
    DeRegisterInterestPack deRegisterPacket;
    deRegisterPacket.timestamp=timestamp;
    //deRegisterPacket.root = root;
    auto encodePair = deRegisterPacket.encode();
    auto packet = make_shared<NdnInterest>();
    packet->setName("/rt/lo/de/" + to_string((unsigned long long)routerID) + "/" + to_string((unsigned long long)parent)+"/"+to_string((unsigned long long)root));
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{neighbor->getInterfaceID(), neighbor->getMacAddress()}});

    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;
    string timerName = "deregister_" + packet->getName();
    IOC->getTimer()->startTimer(timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000, [this, packet, retransmissionTime, interfaceObj](string name) -> bool {
        return getCrobJobHandler()->deRegisterExpireCronJob(retransmissionTime, packet, interfaceObj->getMacAddress(), name);
    });

    // unlock();
    sendPacket(interfaceObj->getMacAddress(), packet);
    // lock();
    return timestamp;
}

void NdnRoutingProtocol::sendInfoToChildren(shared_ptr<LsaDataPack> lsa) {
    RouterID root = lsa->routerID;

    //必须保证遍历过程中map不能被改了，不然必自爆
    for (RouterID parent : minimumHopTree->getRegisteredSons(root)) {
        // send info to each son
        auto neighborObj = getNeighborByRouterID(parent);
        if (neighborObj == nullptr) {
            LOGGER->ERRORF("parent %d not found", parent);
            continue;
        }
        auto interest = lsa->generateInfoInterest();
        interest->setPreferedInterfaces({{neighborObj->getInterfaceID(), neighborObj->getMacAddress()}});
        LOGGER->INFOF(2, "sending info interest %s to %d", interest->getName().c_str(), parent);

        // unlock();
        sendPacket(neighborObj->getBelongingInterface()->getMacAddress(), interest);
        // lock();
    }
}
void NdnRoutingProtocol::sendInfoToAll(shared_ptr<LsaDataPack> lsa, int exemptedIntf) {
    vector<pair<int, MacAddress>> res;
    for (auto interfacePair : interfaces) {
        if (interfacePair.first != exemptedIntf) {
            res.push_back({interfacePair.first, MacAddress("ff:ff:ff:ff:ff:ff")});
        }
    }
    auto interest = lsa->generateInfoInterest();
    interest->setPreferedInterfaces(res);
    if (res.size() == 0) {
        LOGGER->INFOF(2, "stop sending info interest %s because no suitable interfaces", interest->getName().c_str());
        return;
    }
    LOGGER->INFOF(2, "sending info interest %s every interface except%d", interest->getName().c_str(), exemptedIntf);

    // unlock();
    sendPacket(MacAddress("00:00:00:00:00:00"), interest);
    // lock();
}

bool NdnRoutingProtocol::allNeighboursFull() {
    for (auto i : interfaces) {
        for (auto j : i.second->getNeighbors()) {
            if (j.second->getState() != NeighborStateType::FULL) {
                return false;
            }
        }
    }
    return true;
}
int NdnRoutingProtocol::fullNeighborNum() {
    int num=0;
    for (auto i : interfaces) {
        for (auto j : i.second->getNeighbors()) {
            if (j.second->getState()== NeighborStateType::FULL) {
                num++;
            }
        }
    }
    return num;
}
