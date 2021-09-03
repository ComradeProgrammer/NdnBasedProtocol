#include "NdnTransmitter.h"
using namespace std;

std::mutex NdnTransmitter::classStaticLock;
std::shared_ptr<NdnTransmitter> NdnTransmitter::singleInstance;

std::shared_ptr<NdnTransmitter>NdnTransmitter::getTransmitter(std::shared_ptr<Logger> log){
    lock_guard<mutex>lockMethod(classStaticLock);
    if(singleInstance==nullptr){
        NdnTransmitter* tmp=new NdnTransmitter(log);
        singleInstance=shared_ptr<NdnTransmitter>(tmp);
    }
    return singleInstance;
}
NdnTransmitter::NdnTransmitter(std::shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
    // initialize the NIC list
    vector<NIC> allNic = NIC::getAllInterfaces(logger);
    for (int i = 0; i < allNic.size(); i++) {
        nicMap[allNic[i].getInterfaceID()] = allNic[i];
        logger->INFO("NdnTransmitter: detected " + allNic[i].getName() +
                     "at unterface " + to_string(allNic[i].getInterfaceID()) +
                     ", mac address" + allNic[i].getMacAddress().toString());
    }
    // initialize the RawSocket
    rawSocket = make_shared<RawSocket>(logger);
}

void NdnTransmitter::setOnReceivePacket(
    function<void(int interfaceIndex, MacAddress sourceMac,
                  shared_ptr<NdnPacket> packet)>
        _handler) {
    handler = _handler;
}

void NdnTransmitter::send(int interfaceIndex, MacAddress destination,
                          std::shared_ptr<NdnPacket> packet) {
    if (nicMap.find(interfaceIndex) == nicMap.end()) {
        logger->ERROR("NdnTransmitter::send unrecongnized NIC index " +
                      to_string(interfaceIndex));
        return;
    }
    // get source destination
    MacAddress source = nicMap[interfaceIndex].getMacAddress();

    auto rawNdnPacket = packet->encode();

    auto ethernetPacket = make_shared<EthernetPacket>(
        destination, source, NDN_PROTOCOL, rawNdnPacket.second.get(),
        rawNdnPacket.first, logger);

    int res = rawSocket->sendPacket(interfaceIndex, ethernetPacket);

    if (res < 0) {
        logger->ERROR("NdnTransmitter::send: sendout packet to " +
                      destination.toString() + " but return value " +
                      to_string(res));
    } else {
        logger->INFO("NdnTransmitter::send: sendout packet to " +
                     destination.toString());
    }
}

void NdnTransmitter::listen() {
    while (1) {
        auto res = rawSocket->receivePacket(NDN_PROTOCOL);
        auto packet = NdnPacket::decode(res.second->getData(), logger);
        handler(res.first, res.second->getHeader().getDestinationMacAddress(),
                packet);
    }
}
