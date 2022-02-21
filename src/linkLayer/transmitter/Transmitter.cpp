#include "Transmitter.h"
#include"ioc.h"
using namespace std;

void Transmitter::registerNetworkLayerProtocol(int id, shared_ptr<NetworkLayerProtocol> protocol) {
    lock_guard<mutex> lockFuntion(lock);
    protocols[id] = protocol;
}

void Transmitter::onReceiveEthernetPacket(int sourceInterface, int protocolId, shared_ptr<EthernetPacket> packet) {
    lock.lock();
    if (protocols.find(protocolId) == protocols.end()) {
        lock.unlock();
        return;
    }
    auto protocol = protocols[protocolId];
    thread tmp([sourceInterface, packet, protocol]() -> void {
        try {
            protocol->onReceiveEthernetPacket(sourceInterface, packet);
        } catch (exception e) {
                LOGGER->ERRORF("standard exception captured, %s", e.what());
                exit(-1);
            } catch (...) {
                LOGGER->ERROR("non-standard exception captured");
                exit(-1);
            }
    });
    tmp.detach();
    lock.unlock();
}
