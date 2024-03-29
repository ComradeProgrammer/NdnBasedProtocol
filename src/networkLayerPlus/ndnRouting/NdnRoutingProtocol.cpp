#include "NdnRoutingProtocol.h"
using namespace std;
using json = nlohmann::json;

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
            if (splits.size() > 3 && splits[3] == "hello") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "dd") {
                ddController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "LSA") {
                lsaController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "INFO") {
                infoController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "register") {
                registerController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits.size() > 3 && splits[3] == "deregister") {
                deRegisterController->onReceiveInterest(interfaceIndex, sourceMac, interest);
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
            } else if (splits.size() > 3 && splits[3] == "deregister") {
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
    memcpy(lsa->publicKey, publicKey.c_str(), PUBLIC_KEY_LENGTH);
    lsa->signSignature(privateKey);
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
        LOGGER->WARNINGF("NdnRoutingProtocol::sendRegisterPacket parnet %llu not found", parent);
        return timestamp;
    }
    auto interfaceObj = interfaces[neighbor->getInterfaceID()];

    RegisterInterestPack registerPacket;
    registerPacket.root = root;
    auto lsa = database->findLsa(LinkStateType::ADJ, root);
    if (lsa == nullptr) {
        registerPacket.adjSequenceNum = -1;
    } else {
        registerPacket.adjSequenceNum = lsa->seqNum;
    }

    auto encodePair = registerPacket.encode();
    auto packet = make_shared<NdnInterest>();

    packet->setName("/routing/local/register/" + to_string((unsigned long long)routerID) + "/" + to_string((unsigned long long)parent) + "/" +
                    to_string(timestamp));
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{neighbor->getInterfaceID(), neighbor->getMacAddress()}});
    LOGGER->INFOF(2, "sending RegisterInterest %s to %llu for root %llu,content %s", packet->getName().c_str(), parent, root,
                  registerPacket.toString().c_str());

    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;
    string timerName = "register_" + packet->getName();
    IOC->getTimer()->startTimer(timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000, [this, packet, retransmissionTime, interfaceObj](string name) -> bool {
        return getCrobJobHandler()->registerExpireCronJob(retransmissionTime, packet, interfaceObj->getMacAddress(), name);
    });

    AuditEventPacketOut event(getCurrentTime(), neighbor->getInterfaceID(), neighbor->getMacAddress(), parent, AuditEventInterface::INTEREST,
                              AuditEventInterface::REGISTER_PACKET, packet->getName(), registerPacket.marshal());
    IOC->getAuditRecorder()->insertAuditLog(event);

    // unlock();
    sendPacket(interfaceObj->getMacAddress(), packet);
    // lock();
    return timestamp;
}

long NdnRoutingProtocol::sendDeregisterPacket(RouterID root, RouterID parent) {
    LOGGER->INFOF(2, "sending DeregisterInterest to %llu for root %llu", parent, root);
    long timestamp = getTimeStamp();
    auto neighbor = getNeighborByRouterID(parent);
    if (neighbor == nullptr) {
        LOGGER->WARNINGF("NdnRoutingProtocol::sendRegisterPacket parnet %llu not found", parent);
        return timestamp;
    }
    auto interfaceObj = interfaces[neighbor->getInterfaceID()];
    DeRegisterInterestPack deRegisterPacket;
    deRegisterPacket.root = root;
    auto encodePair = deRegisterPacket.encode();
    auto packet = make_shared<NdnInterest>();
    packet->setName("/routing/local/deregister/" + to_string((unsigned long long)routerID) + "/" + to_string((unsigned long long)parent) + "/" +
                    to_string(timestamp));
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{neighbor->getInterfaceID(), neighbor->getMacAddress()}});

    shared_ptr<int> retransmissionTime = make_shared<int>();
    *retransmissionTime = 0;
    string timerName = "deregister_" + packet->getName();
    IOC->getTimer()->startTimer(timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000, [this, packet, retransmissionTime, interfaceObj](string name) -> bool {
        return getCrobJobHandler()->deRegisterExpireCronJob(retransmissionTime, packet, interfaceObj->getMacAddress(), name);
    });

    AuditEventPacketOut event(getCurrentTime(), neighbor->getInterfaceID(), neighbor->getMacAddress(), parent, AuditEventInterface::INTEREST,
                              AuditEventInterface::DEREGISTER_PACKET, packet->getName(), deRegisterPacket.marshal());
    IOC->getAuditRecorder()->insertAuditLog(event);

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
            LOGGER->ERRORF("parent %llu not found", parent);
            continue;
        }
        auto interest = lsa->generateInfoInterest();
        interest->setPreferedInterfaces({{neighborObj->getInterfaceID(), neighborObj->getMacAddress()}});
        LOGGER->INFOF(2, "sending info interest %s to %llu", interest->getName().c_str(), parent);

        AuditEventPacketOut event(getCurrentTime(), neighborObj->getInterfaceID(), neighborObj->getMacAddress(), parent, AuditEventInterface::INTEREST,
                                  AuditEventInterface::INFO_PACKET, interest->getName(), nlohmann::json{});
        IOC->getAuditRecorder()->insertAuditLog(event);

        // unlock();
        sendPacket(neighborObj->getBelongingInterface()->getMacAddress(), interest);
        // lock();
    }
}
void NdnRoutingProtocol::sendInfoToAll(shared_ptr<LsaDataPack> lsa, int exemptedNeighbor) {
    vector<pair<int, MacAddress>> res;
    for (auto interfacePair : interfaces) {
        if (interfacePair.first != exemptedNeighbor) {
            res.push_back({interfacePair.first, MacAddress("ff:ff:ff:ff:ff:ff")});
        }
    }
    auto interest = lsa->generateInfoInterest();
    interest->setPreferedInterfaces(res);
    if (res.size() == 0) {
        LOGGER->INFOF(2, "stop sending info interest %s because no suitable interfaces", interest->getName().c_str());
        return;
    }
    LOGGER->INFOF(2, "sending info interest %s every interface except%llu", interest->getName().c_str(), exemptedNeighbor);

    AuditEventPacketOut event(getCurrentTime(), 0, MacAddress("ff:ff:ff:ff:ff:ff"), 0, AuditEventInterface::INTEREST, AuditEventInterface::INFO_PACKET,
                              interest->getName(), nlohmann::json{});
    IOC->getAuditRecorder()->insertAuditLog(event);

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
    int num = 0;
    for (auto i : interfaces) {
        for (auto j : i.second->getNeighbors()) {
            if (j.second->getState() == NeighborStateType::FULL) {
                num++;
            }
        }
    }
    return num;
}
void NdnRoutingProtocol::generateBlock() {
    while (1) {
        this_thread::sleep_for(std::chrono::milliseconds(10000 + rand() % 10000));
        lock();
        if (blockBuffer.size() == 0) {
            unlock();
            continue;
        }

        stringstream ss;
        for (int i = 0; i < blockBuffer.size(); i++) {
            ss << blockBuffer[i];
            if (i != blockBuffer.size() - 1) {
                ss << ";";
            }
        }
        blockBuffer.clear();
        string assignmentInfo = ss.str();
        json info;
        info["lsa"] = assignmentInfo;

        string tmp = info.dump();
        /*
        LOGGER->INFOF(3, "CHAINOPERATION APPEND: current chain %s", chainToString().c_str());
        chain.generateNewBlock(tmp.c_str(), tmp.size() + 1);
        LOGGER->INFOF(3, "CHAINOPERATION APPEND:after insertion, current chain %s", chainToString().c_str());

        // send out new block
        auto encodePair = encodeBlockChain(&chain);
        char buffer[880];
        auto estimatedHash = chain.chain[chain.chain.size() - 1].getHash();
        string lastHash = estimatedHash.toString().substr(0, 16);

        int packetNum = encodePair.first / 880;
        if (encodePair.first % 880 != 0) {
            packetNum++;
        }
        int remainingSize = encodePair.first;
        char* ptr = encodePair.second.get();
        for (int i = 0; i < packetNum; i++) {
            int dataSize = remainingSize > 880 ? 880 : remainingSize;
            remainingSize -= dataSize;
            auto packet = make_shared<NdnInterest>();
            packet->setName("/addr/broadcast/chain/" + lastHash + "/" + to_string(i) + "/" + to_string(packetNum));
            packet->setNonce(rand());
            memcpy(buffer, ptr, dataSize);
            ptr += dataSize;
            packet->setApplicationParameters(dataSize, buffer);
            LOGGER->INFOF(3, "send out %s", packet->getName().c_str());
            sendPacket(MacAddress("00:00:00:00:00:00"), packet);
        }
        */

        unlock();
    }
}

std::string NdnRoutingProtocol::chainToString() {
    vector<json> array;
    for (int i = 0; i < chain.chain.size(); i++) {
        json jblock;
        jblock["index"] = i;
        auto data = chain.chain[i].getData();
        if (chain.chain[i].getDataSize() != 0) {
            jblock["info"] = json::parse(string(data));
        }
        array.push_back(jblock);
    }

    json res = array;
    return res.dump();
}