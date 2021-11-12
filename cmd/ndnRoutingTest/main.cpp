#include <iostream>

#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "util/log/FileLogger.h"
#include "ethernet/interface/NICManager.h"

using namespace std;

int main(int argc, char* argv[]) {
    struct timeval tm;
    gettimeofday(&tm, NULL);
    int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    srand(seed);

    string name(argv[1]);
    auto logger = make_shared<FileLogger>(name + ".log");
    NICManager::getNICManager()->setPrefix(name + "-");

    NICManager::getNICManager()->getAllInterfaces();

    logger->INFO(name + " ndn protocol start, seed " + to_string(seed));
    auto trans = NdnTransmitter::getTransmitter(logger);
    auto protocol = NdnProtocol::getNdnProtocol(logger);

    trans->setOnReceivePacket([protocol](int interfaceIndex,
                                         MacAddress sourceMac,
                                         shared_ptr<NdnPacket> packet) -> void {
        protocol->onIncomingPacket(interfaceIndex, sourceMac, packet);
    });

    // start ndn Routing
    auto ndnRoutingProtocol = NdnRoutingProtocol::getNdnRoutingProtocol(logger);

    ndnRoutingProtocol->lock();
    ndnRoutingProtocol->setRouterID(atoi(name.substr(1, 1).c_str()));
    ndnRoutingProtocol->unlock();
    NICManager::getNICManager()->startMonitor();
    ndnRoutingProtocol->initialize();
    


    // ndnRoutingProtocol->lock();
    // //for test
    // if(name=="s1"){ 
    //     shared_ptr<LsaDataPack> p2=make_shared<LsaDataPack>();
    //     p2->lsType=LinkStateType::RCH;
    //     p2->routerID=1;
    //     p2->seqNum=2;
    //     p2->lsAge=256;
    //     p2->numberOfLinks=0;
    //     logger->INFO("here0");
    //     ndnRoutingProtocol->database.insertLsa(p2);
    //     logger->INFO("here1");
    // }
    // ndnRoutingProtocol->unlock();

    thread recv([name, trans, logger]() -> void {
        logger->INFO(name + " recv thread start");
        trans->listen();
    });
    
    recv.join();
}