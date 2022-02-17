#include"CronJobHandler.h"
#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include"networkLayerPlus/ndnRouting/dataPack/HelloInterestPack.h"
using namespace std;
void CronJobHandler::sendingHelloMessageCronJob(int interfaceIndex){
    protocol->lock->lock();

    auto interfaceObj=protocol->interfaces[interfaceIndex];
    HelloInterestPack helloPack;
    helloPack.routerId = protocol->getRouterID();
    helloPack.interfaceIP = interfaceObj->getIpv4Address();
    helloPack.networkMask = interfaceObj->getIpv4Mask();
    helloPack.helloInterval = NDNROUTING_HELLOINTERVAL;
    helloPack.routerDeadInterval = NDNROUTING_ROUTERDEADINTERVAL;
    for(auto neighbor:interfaceObj->neighbors){
        helloPack.neighbor.push_back(neighbor.second->getIpv4Address());
    }

    auto encodePair = helloPack.encode();
    auto packet = make_shared<NdnInterest>();
    packet->setName("/routing/local/hello");
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});
    
    protocol->lock->unlock();
    protocol->sendPacket(interfaceObj->getMacAddress(),packet);
}   

void CronJobHandler::neighborInactivityCronJob(NdnRoutingNeighbor*neighbor){
    protocol->lock->lock();
    neighbor->processEvent(NeighborEventType::INACTIVITY_TIMER);
    protocol->lock->unlock();
}