#include"CronJobHandler.h"
#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include"networkLayerPlus/ndnRouting/dataPack/HelloInterestPack.h"
using namespace std;
void CronJobHandler::sendingHelloMessageCronJob(int interfaceIndex){
    LOGGER->VERBOSE("here1");
    protocol->lock->lock();

    auto interface=protocol->interfaces[interfaceIndex];
    HelloInterestPack helloPack;
    helloPack.routerId = protocol->getRouterID();
    helloPack.interfaceIP = interface->getIpv4Address();
    helloPack.networkMask = interface->getIpv4Mask();
    helloPack.helloInterval = NDNROUTING_HELLOINTERVAL;
    helloPack.routerDeadInterval = NDNROUTING_ROUTERDEADINTERVAL;
    // for (auto tmp : neighbors) {
    //     helloPack.neighbor.push_back(tmp.second->getIpAddress());
    // }

    auto encodePair = helloPack.encode();
    auto packet = make_shared<NdnInterest>();
    packet->setName("/routing/local/hello");
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});
    
    protocol->lock->unlock();
    protocol->sendPacket(interface->getMacAddress(),packet);
    LOGGER->VERBOSE("here2");
}   