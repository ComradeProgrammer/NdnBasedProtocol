#include"NdnRoutingProtocol.h"
using namespace std;
NdnRoutingProtocol::NdnRoutingProtocol(RouterID _routerID,std::shared_ptr<NdnProtocol> _ndnProtocol) : routerID(_routerID), ndnProtocol(_ndnProtocol){
    cronJobHandler=make_shared<CronJobHandler>(this);
    helloController=make_shared<HelloController>(this);
}
void NdnRoutingProtocol::onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnPacket> packet){
    auto splits = split(packet->getName(), "/");
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "hello") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
            } else if (splits[3] == "dd") {
               // onReceiveDDInterest(interfaceIndex, sourceMac, interest);
            } else if (splits[3] == "LSA") {
               // onReceiveLsaInterest(interfaceIndex, sourceMac, interest);
            } else if (splits[3] == "INFO") {
               // onReceiveInfoInterest(interfaceIndex, sourceMac, interest);
            }

            break;
        }
    }
}

void NdnRoutingProtocol::start(){
    lock=make_shared<mutex>();
    //establish interface structure
    auto nics=IOC->getNicManager()->getAllNicsInMap();
    for(auto nic:nics){
        LOGGER->INFO("here1");
        auto interface=make_shared<NdnRoutingInterface>(this);
        LOGGER->INFO("here2");
        interface->setName(nic.second->getName());
        interface->setInterfaceID(nic.second->getInterfaceID());
        interface->setMacAddress(nic.second->getMacAddress());
        interface->setIpv4Address(nic.second->getIpv4Address());
        interface->setIpv4Mask(nic.second->getIpv4Mask());
        LOGGER->INFO("here3");
        IOC->getNicManager()->registerObserver(interface.get(),interface->getInterfaceID());
        interfaces[interface->getInterfaceID()]=interface;
        LOGGER->INFO("here4");
        interface->processInterfaceEvent(NdnRoutingInterfaceEventType::INTERFACE_UP);
    }

        
}

void NdnRoutingProtocol::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    ndnProtocol->onReceiveNdnPacket(NDN_ROUTING, sourceMac, packet);
    return;
}