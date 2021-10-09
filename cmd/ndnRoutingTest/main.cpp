#include<iostream>
#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "util/log/FileLogger.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"
using namespace std;

int main(int argc, char* argv[]){
    struct timeval tm;

    gettimeofday(&tm, NULL);
    int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;  
    srand(seed); 
    string name(argv[1]);
    NIC::setPrefix(name + "-");

    auto logger = make_shared<FileLogger>(name + ".log");
    logger->INFO(name + " ndn protocol start, seed "+to_string(seed));
    auto trans = NdnTransmitter::getTransmitter(logger);
    auto protocol =NdnProtocol::getNdnProtocol(logger);

    trans->setOnReceivePacket([protocol](int interfaceIndex,
                                         MacAddress sourceMac,
                                         shared_ptr<NdnPacket> packet) -> void {
        protocol->onIncomingPacket(interfaceIndex, sourceMac, packet);
    });

    //start ndn Routing
    auto ndnRoutingProtocol=NdnRoutingProtocol::getNdnRoutingProtocol(logger);
    ndnRoutingProtocol->lock();
    ndnRoutingProtocol->setRouterID(atoi(name.substr(1,1).c_str()));
    ndnRoutingProtocol->unlock();


    thread recv([name, trans, logger]() -> void {
        logger->INFO(name + " recv thread start");
        trans->listen();
    });
    ndnRoutingProtocol->initialize();
    recv.join();

}