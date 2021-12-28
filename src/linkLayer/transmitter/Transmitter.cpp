#include"Transmitter.h"
using namespace std;


void Transmitter::registerNetworkLayerProtocol(int id,shared_ptr<NetworkLayerProtocol>protocol){
    lock_guard<mutex>lockFuntion(lock);
    protocols[id]=protocol;
}

void Transmitter::onReceiveEthernetPacket(int sourceInterface, int protocolId, shared_ptr<EthernetPacket>packet){
    lock.lock();
    if(protocols.find(protocolId)==protocols.end()){
        return;
    }
    auto protocol=protocols[protocolId];
    //release the lock because the onReceiveEthernetPacket method may not return quickly
    lock.unlock();
    protocol->onReceiveEthernetPacket(sourceInterface,packet);
}

