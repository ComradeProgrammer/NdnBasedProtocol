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
    auto manager= NICManager::getNICManager();
    auto nics =manager->getAllInterfaces();
    for (auto nic : nics) {
        auto neighbor=make_shared<NdnRoutingInterface>(nic, logger);
        interfaces[nic.getInterfaceID()] = neighbor;
        manager->registerObserver(neighbor.get());
    }
    // turn the up interfaces into UP state
    for (auto nic : nics) {
        interfaces[nic.getInterfaceID()]->processStateEvent(NdnRoutingInterfaceEventType::INTERFACE_UP);
    }
    database=LsaDataBase(logger);
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
            } else if(splits[3]=="INFO"){
                onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
        case TLV_DATA: {
            auto data = dynamic_pointer_cast<NdnData>(packet);
            if (splits[3] == "dd") {
                onReceiveDDData(interfaceIndex, sourceMac, data);
            }else if(splits[3]=="LSA"){
                onReceiveLsaData(interfaceIndex, sourceMac, data);
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
            auto existingLsa = database.findLsa(LinkStateType::ADJ, lsa->routerID);
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
            auto existingLsa = database.findLsa(LinkStateType::RCH, lsa->routerID);
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
            if(neighbor.second->getState()<NeighborStateType::EXCHANGE_STATE){
                continue;
            }
            neighbor.second->cancelLsaInterestRequest(lsa->generateLSDigest());
        }
    }else{
        //TODO: handle broadcast lsa
        list<LinkStateDigest>::iterator itr;
        for(itr=broadcastLsaPendingRequestList.begin();itr!=broadcastLsaPendingRequestList.end();itr++){
            if(itr->routerID==lsa->routerID){
                broadcastLsaPendingRequestList.erase(itr);
                break;
            }
        }
        //todo: remove retransmission timer once retransmiting mechanism is started
        
        //Timer::GetTimer()->cancelTimer(timerName);


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

shared_ptr<LsaDataPack> NdnRoutingProtocol::generateLsa(){
    shared_ptr<LsaDataPack>lsa=make_shared<LsaDataPack>();
    lsa->lsType=LinkStateType::ADJ;
    lsa->routerID=routerID;
    lsa->seqNum=0;
    lsa->lsAge=NDN_ROUTING_MAX_AGE;
    lsa->numberOfLinks=0;//will be increased
    for(auto interfacePair: interfaces){
        if(interfacePair.second->getState()->getState()==NdnRoutingInterfaceStateType::DOWN){
            continue;
        }
        for(auto neighborPair: interfacePair.second->neighbors){
            NdnLink link;
            link.linkType=LinkType::TRANSIT_LINK;
            link.linkID=neighborPair.second->getRouterID();
            link.linkData=neighborPair.second->getIpAddress().addr;
            link.linkDataMask=neighborPair.second->getIpMask().addr;
            link.linkCost=interfacePair.second->getCost();
            lsa->numberOfLinks++;
            lsa->links.push_back(link);
        }
    }
    return lsa;
   
}
void NdnRoutingProtocol::onReceiveInfoInterest(int interfaceIndex, MacAddress sourceMac,shared_ptr<NdnInterest>interest){
    //extract the info message from the packet
    InfoInterestPack infoInterest;
    auto encodingPair=interest->getApplicationParameters();
    infoInterest.decode(encodingPair.second.get(), encodingPair.first);
    logger->INFOF("NdnRoutingProtocol::onReceiveInfoInterest:interest %s",infoInterest.toString().c_str());

    //todo: figure out pxf's codes' function

    //send out broadcast interest
    for(auto digest: infoInterest.ls){
        auto existingLsa=findLsa(digest.linkStateType,digest.routerID);
        //check whether the lsa described in info is new
        if(existingLsa!=nullptr && ((digest<existingLsa->generateLSDigest()))){
            logger->INFO("NdnRoutingProtocol::onReceiveInfoInterest more recent lsa found");
            continue;
        }
       

        //check whether the lsa has already be requested
        for(auto i: broadcastLsaPendingRequestList){
            if(i.routerID==digest.routerID){
                logger->INFO("NdnRoutingProtocol::onReceiveInfoInterest has been requested");
                continue;
            }
        }


        //fine, we need to send interest for it
        sendBroadcastLsaInterest(digest, interfaceIndex);
    }

}

void NdnRoutingProtocol::sendBroadcastLsaInterest(LinkStateDigest digest,int interface){

    string name="/routing/broadcast/LSA/"+getNameForLinkStateType(digest.linkStateType)+"/"+to_string(digest.routerID)+"/"+to_string(digest.sequenceNum);
    LsaInterestPack lsaInterestPack;
    lsaInterestPack.routerID=digest.routerID;
    lsaInterestPack.sequenceNum=digest.sequenceNum;
    lsaInterestPack.lsType=digest.linkStateType;

    auto encodePair=lsaInterestPack.encode();

    auto packet = make_shared<NdnInterest>(logger);
    packet->setName(name);
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first,encodePair.second.get());
    packet->setPreferedInterfaces(
        {{interface, MacAddress("ff:ff:ff:ff:ff:ff")}});

    //unlock first because sendPacket will attain the lock of ndnprotocol
    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(MacAddress("ff:ff:ff:ff:ff:ff"), packet);
    //get the lock back because after return the lock needs to be attained
    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();
}
