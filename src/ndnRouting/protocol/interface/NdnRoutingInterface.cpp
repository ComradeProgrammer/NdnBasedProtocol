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
    logger->INFOF("Interface State changed on interface %d , from %s to %s",interfaceID,getNameForInterfaceStateType(state->getState()).c_str(),getNameForInterfaceStateType(newStateType).c_str());
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
    logger->INFOF(
        "interface %d process event %s on state %s",
        interfaceID,
        getNameForInterfaceEventType(event).c_str(),
        getNameForInterfaceStateType(state->getState()).c_str()
    );
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
    for(auto tmp:neighbors){
        helloPack.neighbor.push_back(tmp.second->getIpAddress());
    }

    auto encodePair = helloPack.encode();
    auto packet = make_shared<NdnInterest>(logger);
    packet->setName("/routing/local/hello");
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces(
        {{interfaceID, MacAddress("ff:ff:ff:ff:ff:ff")}});

    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(macAddress, packet);
}

void NdnRoutingInterface::clear(){
    //TODO: implement
}

void NdnRoutingInterface::onReceiveHelloInterest(MacAddress addr, std::shared_ptr<NdnInterest> interest){
    //fetch the hello interest content
    auto helloInfoData=interest->getApplicationParameters();
    HelloInterestPack helloInfo;
    helloInfo.decode(helloInfoData.second.get(),helloInfoData.first);
    //check the mask, helloInterval and routerDeadInterval
    if(!(ipv4Mask==helloInfo.networkMask)){
        logger->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible mask");
        return;
    }
    if(helloInfo.helloInterval!=NDNROUTING_HELLOINTERVAL){
        logger->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible hello interval");
        return;
    }
    if(helloInfo.routerDeadInterval!=NDNROUTING_ROUTERDEADINTERVAL){
        logger->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible router dead interval");
        return;
    }
    //if related neighbor is not recorded, create a new one
    if(neighbors.find(helloInfo.routerId)==neighbors.end()){
        shared_ptr<NdnRoutingNeighbor>newNeighbor=make_shared<NdnRoutingNeighbor>(this,logger);
        newNeighbor->setRouterID(helloInfo.routerId);
        newNeighbor->setMacAddress(addr);
        newNeighbor->setIpAddress(helloInfo.interfaceIP);
        newNeighbor->setIpMask(helloInfo.networkMask);
        neighbors[helloInfo.routerId]=newNeighbor;
    }
    auto neighbor=neighbors[helloInfo.routerId];
    neighbor->processEvent(HELLO_RECEIVED);
    //now resolve the neighbor info incorporated in the hello packet
    //if we find that current instance shows up in hellopacket, then just go directly into 2-way
    bool found=false;
    for(auto i:helloInfo.neighbor){
        if(i==ipv4Addr){
            //found
            neighbor->processEvent(TWOWAY_RECEIVED);
            found=true;
            break;
        }
    }
    if(!found){
        neighbor->processEvent(ONEWAY_RECEIVED);
    }
}
