#include "NdnAddrAssignment.h"
using namespace std;
using json = nlohmann::json;

NdnAddrAssignmentProtocol::NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol)
    : routerID(_routerID), ndnProtocol(_ndnProtocol) {
    cronjobController = make_shared<AddrCronjobController>(this);
    helloController = make_shared<AddrHelloController>(this);
    requestController = make_shared<AddrRequestController>(this);
    confirmationController = make_shared<AddrConfirmationController>(this);
    mutexLock = make_shared<mutex>();
}

void NdnAddrAssignmentProtocol::onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    auto splits = split(packet->getName(), "/");
    switch (packet->getPacketType()) {
        case TLV_INTEREST: {
            LOGGER->INFOF(3, "NdnaddrProtocol INTEREST %s received, content %s from interface %d", packet->getName().c_str(), packet->toString().c_str(),
                          interfaceIndex);
            auto interest = dynamic_pointer_cast<NdnInterest>(packet);
            if (splits.size() > 3 && splits[3] == "hello") {
                helloController->onReceiveInterest(interfaceIndex, sourceMac, interest);
                return;
            } else if (splits.size() > 3 && splits[3] == "req") {
                requestController->onReceiveInterest(interfaceIndex, sourceMac, interest);
                return;
            } else if (splits.size() > 3 && splits[3] == "conf") {
                confirmationController->onReceiveInterest(interfaceIndex, sourceMac, interest);
                return;
            }
        }
        case TLV_DATA: {
            auto data = dynamic_pointer_cast<NdnData>(packet);
            if (splits.size() > 3 && splits[3] == "req") {
                requestController->onReceiveData(interfaceIndex, sourceMac, data);
                return;
            }
        }
    }
}

void NdnAddrAssignmentProtocol::start() {
    // genesis block;
    chain.generateNewBlock("", 0);
    auto nics = IOC->getNicManager()->getAllNicsInMap();
    for (auto nic : nics) {
        int intfID = nic.first;
        // create interfaces
        auto interface = make_shared<NdnAddrInterface>(this);
        interface->setName(nic.second->getName());
        interface->setInterfaceID(nic.second->getInterfaceID());
        interface->setMacAddress(nic.second->getMacAddress());
        interface->setIpv4Address(nic.second->getIpv4Address());
        interface->setIpv4Mask(nic.second->getIpv4Mask());
        IOC->getNicManager()->registerObserver(interface.get(), interface->getInterfaceID());
        interfaces[interface->getInterfaceID()] = interface;
        interface->processInterfaceEvent(NdnAddrInterfaceEventType::INTERFACE_UP);
    }
    thread daemon([this]() -> void {
        generateBlock();
    });
    daemon.detach();
}

void NdnAddrAssignmentProtocol::sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) {
    ndnProtocol->onReceiveNdnPacket(NDN_ADDRASSIGNMENT, sourceMac, packet);
    return;
}

std::string NdnAddrAssignmentProtocol::chainToString() {
    vector<json> array;
    for (int i = 0; i < chain.chain.size(); i++) {
        json jblock;
        jblock["index"] = i;
        auto data = chain.chain[i].getData();
        if (chain.chain[i].getDataSize() != 0) {
            jblock["info"] = string(data);
        }
        array.push_back(jblock);
    }

    json res = array;
    return res.dump();
}

void NdnAddrAssignmentProtocol::generateBlock() {
    while (1) {
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        lock();
        if (blockBuffer.size() == 0) {
            unlock();
            continue;
        }

        //fake workproof
        int interval=rand()%2000;
        this_thread::sleep_for(std::chrono::milliseconds(interval));

        stringstream ss;
        for (int i = 0; i < blockBuffer.size(); i++) {
            ss << blockBuffer[i];
            if (i != blockBuffer.size()-1) {
                ss << ";";
            }
        }
        blockBuffer.clear();
        string assignmentInfo = ss.str();
        LOGGER->INFOF(3, "CHAINOPERATION: current chain %s", chainToString().c_str());
        chain.generateNewBlock(assignmentInfo.c_str(), assignmentInfo.size() + 1);
        LOGGER->INFOF(3, "CHAINOPERATION:after insertion, current chain %s", chainToString().c_str());
        unlock();
    }
}
