#include "NdnRoutingProtocol.h"
using namespace std;
shared_ptr<NdnRoutingProtocol> NdnRoutingProtocol::protocol = nullptr;
mutex NdnRoutingProtocol::staticLock;

shared_ptr<NdnRoutingProtocol> NdnRoutingProtocol::getNdnRoutingProtocol(shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(staticLock);
    if (protocol == nullptr) {
        protocol = shared_ptr<NdnRoutingProtocol>(new NdnRoutingProtocol(_logger));
    }
    return protocol;
}

NdnRoutingProtocol::NdnRoutingProtocol(shared_ptr<Logger> _logger) { logger = Logger::getDefaultLoggerIfNull(_logger); }

void NdnRoutingProtocol::lock() {
    // logger->INFO("lock required");
    syncLock.lock();
    // logger->INFO("lock required succeeded");
}
void NdnRoutingProtocol::unlock() {
    // logger->INFO("unlock required");

    syncLock.unlock();
    // logger->INFO("unlock required succeeded");
}
void NdnRoutingProtocol::initialize() {
    // 0. register to ndn layer
    NdnProtocol::getNdnProtocol()->registerUpperLayerProtocol(
        NDN_ROUTING, [this](int intf, MacAddress addr, shared_ptr<NdnPacket> handler) -> void {
            onReceivePacket(intf, addr, handler);
        });
    lock();
    // 1.establish interface data structure
    auto nics = NIC::getAllInterfaces();
    for (auto nic : nics) {
        interfaces[nic.getInterfaceID()] = make_shared<NdnRoutingInterface>(nic, logger);
    }
    // turn the up interfaces into UP state
    for (auto nic : nics) {
        interfaces[nic.getInterfaceID()]->processStateEvent(INTERFACE_UP);
    }
    unlock();
}

void NdnRoutingProtocol::onReceivePacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    logger->INFOF(
        "NdnRoutingProtocol::onReceivePacket packet received %s, from "
        "interface %d, source mac %s",
        packet->toString().c_str(), interfaceIndex, sourceMac.toString().c_str());
    lock();

    auto splits = split(packet->getName(), "/");
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "hello") {
                onReceiveHelloInterest(interfaceIndex, sourceMac, interest);
            } else if (splits[3] == "dd") {
                onReceiveDDInterest(interfaceIndex, sourceMac, interest);
            } else if (splits[3] == "LSA") {
                onReceiveLsaInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
        case TLV_DATA: {
            auto data = dynamic_pointer_cast<NdnData>(packet);
            if (splits[3] == "dd") {
                onReceiveDDData(interfaceIndex, sourceMac, data);
            }
            break;
        }
    }
    unlock();
}

void NdnRoutingProtocol::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    NdnProtocol::getNdnProtocol()->onIncomingPacket(NDN_ROUTING, sourceMac, packet);
    return;
}

void NdnRoutingProtocol::onReceiveHelloInterest(int interfaceIndex, MacAddress sourceMac,
                                                std::shared_ptr<NdnInterest> interest) {
    logger->INFOF("NdnRoutingProtocol::onReceiveHelloInterest %s", sourceMac.toString().c_str());
    interfaces[interfaceIndex]->onReceiveHelloInterest(sourceMac, interest);
}

void NdnRoutingProtocol::onReceiveDDInterest(int interfaceIndex, MacAddress sourceMac,
                                             std::shared_ptr<NdnInterest> interest) {
    interfaces[interfaceIndex]->onReceiveDDInterest(sourceMac, interest);
}

void NdnRoutingProtocol::onReceiveDDData(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnData> data) {
    interfaces[interfaceIndex]->onReceiveDDData(sourceMac, data);
}
void NdnRoutingProtocol::onReceiveLsaData(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnData> data) {
    shared_ptr<LsaDataPack> lsa = make_shared<LsaDataPack>();
    auto tmp = data->getContent();
    lsa->decode(tmp.second.get(), tmp.first);

    bool rebuild = false;
    switch (lsa->lsType) {
        case LinkStateType::ADJ: {
            auto existingLsa = database.findLsa(ADJ, lsa->routerID);
            if (existingLsa == nullptr) {
                database.insertLsa(lsa);
                rebuild = true;
            } else {
                if (existingLsa->generateLSDigest() < lsa->generateLSDigest()) {
                    database.insertLsa(lsa);
                    rebuild = true;
                }
            }
            break;
        }
        case LinkStateType::RCH: {
            auto existingLsa = database.findLsa(RCH, lsa->routerID);
            if (existingLsa == nullptr) {
                database.insertLsa(lsa);
                rebuild = true;
            } else {
                if (existingLsa->generateLSDigest() < lsa->generateLSDigest()) {
                    database.insertLsa(lsa);
                    rebuild = true;
                }
                break;
            }
        }
    }
    auto splits = split(data->getName(), "/");
    if(splits[2]=="local"){
        auto interface=interfaces[interfaceIndex];
        for(auto neighbor: interface->neighbors){
            if(neighbor.second->getState()<EXCHANGE_STATE){
                continue;
            }
            neighbor.second->cancelLsaInterestRequest(lsa->generateLSDigest());
        }
    }else{
        //TODO: handle broadcast lsa
    }
    if(rebuild){
        database.rebuildRoutingTable();
    }
}
void NdnRoutingProtocol::onReceiveLsaInterest(int interfaceIndex, MacAddress sourceMac,
                                              std::shared_ptr<NdnInterest> interest) {
    interfaces[interfaceIndex]->onReceiveLsaInterest(sourceMac, interest);
}

shared_ptr<LsaDataPack> NdnRoutingProtocol::findLsa(LinkStateType lsaType, uint32_t routerID) {
    return database.findLsa(lsaType, routerID);
}

bool NdnRoutingProtocol::inBroadcastLsaPendingRequestList(LinkStateType lsaType, uint32_t routerID,
                                                          uint32_t sequenceNum) {
    for (auto digest : broadcastLsaPendingRequestList) {
        if (digest.linkStateType == lsaType && digest.routerID == routerID && digest.sequenceNum == sequenceNum) {
            return true;
        }
    }
    return false;
}