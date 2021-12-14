#include <iostream>
#include "util/log/FileLogger.h"
#include "ethernet/interface/NICManager.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "ndnAddr/protocol/NdnAddressAssignment.h"
using namespace std;
int main(int argc, char* argv[]){
    struct timeval tm;
    gettimeofday(&tm, NULL);
    int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    srand(seed);

    string name(argv[1]);
    auto logger = make_shared<FileLogger>(name + ".log");
    NICManager::getNICManager()->setPrefix(name + "-");
    //set ndn
    auto trans = NdnTransmitter::getTransmitter(logger);
    auto protocol = NdnProtocol::getNdnProtocol(logger);
    trans->setOnReceivePacket(
        [protocol](int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnPacket> packet) -> void {
            protocol->onIncomingPacket(interfaceIndex, sourceMac, packet);
        });  

    thread recv([name, trans, logger]() -> void {
        logger->INFO(name + " recv thread start");
        trans->listen();
    });
    
    auto addrProtocol=NdnAddressAssignment::getNdnAddressAssignment(logger);
    addrProtocol->lock();
    addrProtocol->setRouterID(atoi(name.substr(1, 1).c_str()));
    addrProtocol->unlock();  

    NICManager::getNICManager()->startMonitor();
    addrProtocol->initialize();

    

    recv.join();
    return 0;
}