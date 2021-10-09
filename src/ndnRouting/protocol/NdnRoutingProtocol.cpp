#include "NdnRoutingProtocol.h"
using namespace std;
shared_ptr<NdnRoutingProtocol> NdnRoutingProtocol::protocol = nullptr;
mutex NdnRoutingProtocol::staticLock;

shared_ptr<NdnRoutingProtocol> NdnRoutingProtocol::getNdnRoutingProtocol(
    shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(staticLock);
    if (protocol == nullptr) {
        protocol =
            shared_ptr<NdnRoutingProtocol>(new NdnRoutingProtocol(_logger));
    }
    return protocol;
}

NdnRoutingProtocol::NdnRoutingProtocol(shared_ptr<Logger> _logger) {
    logger = Logger::getDefaultLoggerIfNull(_logger);
}


void NdnRoutingProtocol::lock(){
    syncLock.lock();
}
void NdnRoutingProtocol::unlock(){
    syncLock.unlock();
}
void NdnRoutingProtocol::initialize() {
    //0. register to ndn layer
    NdnProtocol::getNdnProtocol()->registerUpperLayerProtocol(NDN_ROUTING,[this](int intf,MacAddress addr, shared_ptr<NdnPacket>handler)->void{
        onReceivePacket(intf,addr,handler);
    });
    lock();
    // 1.establish interface data structure
    auto nics=NIC::getAllInterfaces();
    for(auto nic:nics){
        interfaces[nic.getInterfaceID()]=make_shared<NdnRoutingInterface>(nic,logger);
    }
    //turn the up interfaces into UP state
    for(auto nic:nics){
        interfaces[nic.getInterfaceID()]->processStateEvent(INTERFACE_UP);
    }
    unlock();

}

void NdnRoutingProtocol::onReceivePacket(int interfaceIndex, MacAddress sourceMac,std::shared_ptr<NdnPacket>packet){
   logger->INFOF("NdnRoutingProtocol::onReceivePacket packet received %s",packet->toString().c_str());
}

void NdnRoutingProtocol::sendPacket( MacAddress sourceMac,std::shared_ptr<NdnPacket>packet ){
    NdnProtocol::getNdnProtocol()->onIncomingPacket(NDN_ROUTING,sourceMac,packet);
}