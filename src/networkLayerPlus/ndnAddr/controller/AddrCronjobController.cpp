#include "AddrCronjobController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
using namespace std;
void AddrCronjobController::sendingHelloMessageCronJob(int interfaceIndex) {
    try {
        protocol->lock();
        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERROR("AddrCronjobController::sendingHelloMessageCronJob: interface not found");
            return;
        }
        AddrHelloInterest helloInterest;
        helloInterest.routerID = protocol->routerID;
        helloInterest.leader = interfaceObj->getLeader();
        helloInterest.addr = interfaceObj->getIpv4Address();
        helloInterest.mask = interfaceObj->getIpv4Mask();
        helloInterest.interfaceNum = protocol->interfaces.size();
        helloInterest.helloInterval = NDNADDR_HELLOINTERVAL;

        auto packet = make_shared<NdnInterest>();
        auto encodePair = helloInterest.encode();
        packet->setName("/addr/local/hello");
        packet->setNonce(rand());
        packet->setApplicationParameters(encodePair.first, (const char*)encodePair.second.get());
        packet->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});

        protocol->sendPacket(interfaceObj->getMacAddress(), packet);
        protocol->unlock();
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}