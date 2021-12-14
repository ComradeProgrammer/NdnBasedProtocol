#include "NdnAddressAssignment.h"
using namespace std;
mutex NdnAddressAssignment::staticLock;
shared_ptr<NdnAddressAssignment> NdnAddressAssignment::staticPtr = nullptr;
shared_ptr<NdnAddressAssignment> NdnAddressAssignment::getNdnAddressAssignment(std::shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(staticLock);
    if (staticPtr == nullptr) {
        NdnAddressAssignment* tmp = new NdnAddressAssignment(_logger);
        staticPtr = shared_ptr<NdnAddressAssignment>(tmp);
    }
    return staticPtr;
}

NdnAddressAssignment::NdnAddressAssignment(shared_ptr<Logger> _logger) {
    logger = Logger::getDefaultLoggerIfNull(_logger);
}

void NdnAddressAssignment::lock() { objectLock.lock(); }

void NdnAddressAssignment::unlock() { objectLock.unlock(); }

void NdnAddressAssignment::initialize() {
    // 0. register to ndn layer
    NdnProtocol::getNdnProtocol()->registerUpperLayerProtocol(
        NDN_ADDRASSIGNMENT, [this](int intf, MacAddress addr, shared_ptr<NdnPacket> handler) -> void {
            onReceivePacket(intf, addr, handler);
        });
    lock();
    // 1.establish interface data structure
    auto manager = NICManager::getNICManager();
    auto nics = manager->getAllInterfaces();
    for (auto nic : nics) {
        auto interface = make_shared<NdnAddrInterface>(nic, logger);
        interfaces[nic.getInterfaceID()] = interface;
        manager->registerObserver(interface.get());
    }
    // 2 turn the up interfaces into UP state
    // todo: implement
    unlock();

}

void NdnAddressAssignment::onReceivePacket(int interfaceIndex, MacAddress sourceMac,std::shared_ptr<NdnPacket> packet) {
    logger->INFOF(
        "NdnAddressAssignment::onReceivePacket packet received %s, from "
        "interface %d, source mac %s",
        packet->toString().c_str(), interfaceIndex, sourceMac.toString().c_str());

    lock();
    unlock();
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            break;
        }
        case TLV_DATA: {
            break;
        }
    }
}
void NdnAddressAssignment::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    NdnProtocol::getNdnProtocol()->onIncomingPacket(NDN_ADDRASSIGNMENT, sourceMac, packet);
    return;
}