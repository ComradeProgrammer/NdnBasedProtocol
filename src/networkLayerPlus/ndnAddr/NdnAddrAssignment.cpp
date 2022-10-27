#include "NdnAddrAssignment.h"
using namespace std;
using json = nlohmann::json;

NdnAddrAssignmentProtocol::NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol)
    : routerID(_routerID), ndnProtocol(_ndnProtocol) {
    cronjobController = make_shared<AddrCronjobController>(this);
    helloController = make_shared<AddrHelloController>(this);
    requestController = make_shared<AddrRequestController>(this);
    confirmationController = make_shared<AddrConfirmationController>(this);
    chainController = make_shared<AddrChainController>(this);
    mutexLock = make_shared<mutex>();
}
string chainToString2(BlockChain chain) {
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
            } else if (splits.size() > 3 && splits[3] == "chain") {
                chainController->onReceiveInterest(interfaceIndex, sourceMac, interest);
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
    if (isRoot) {
        thread daemon([this]() -> void { generateBlock(); });
        daemon.detach();
    }

    for (auto nic : nics) {
        int intfID = nic.first;
        // create interfaces
        auto interface = make_shared<NdnAddrInterface>(this);
        interface->setName(nic.second->getName());
        interface->setInterfaceID(nic.second->getInterfaceID());
        interface->setMacAddress(nic.second->getMacAddress());
        // interface->setIpv4Address(nic.second->getIpv4Address());
        // interface->setIpv4Mask(nic.second->getIpv4Mask());
        // interface->setIpv4Mask(Ipv4Address("255.255.255.0"));
        // interface->setIpv4Address(Ipv4Address("10.0.0.1"));
        // interface->flushIpAddress();
        IOC->getNicManager()->registerObserver(interface.get(), interface->getInterfaceID());
        interfaces[interface->getInterfaceID()] = interface;
    }

    // this_thread::sleep_for(std::chrono::milliseconds(5000));

    for (auto nic : nics) {
        int intfID = nic.second->getInterfaceID();
        auto interface = interfaces[intfID];
        this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
        interface->processInterfaceEvent(NdnAddrInterfaceEventType::INTERFACE_UP);
    }
    startTime = getTimeStamp();
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



// void NdnAddrAssignmentProtocol::generateRootInfoBlock() {
//     while(1){
//         // BUG: there is a concurrency problem here

//     int interval = rand() % 2000;
//     this_thread::sleep_for(std::chrono::milliseconds(interval + routerID*100));
//     int pow = proveOfWork(1, chain.chain[chain.chain.size() - 1].getHash());

//     for(int i=1;i<chain.chain.size();i++){
//         if(chain.chain[i].getHash()==rootBlockHash){
//             continue;
//         }
//     }
//     lock_guard<mutex>(*mutexLock);

//     json info;
//     info["type"] = "rootinfo";
//     info["certificate"] = "";
//     info["workproof"] = pow;
//     auto p = addressPool->getPool();
//     info["startaddr"] = p.first.toString();
//     info["poolmask"] = p.second.toString();

//     string tmp = info.dump();
//     LOGGER->INFOF(3, "CHAINOPERATION APPEND: current chain %s", chainToString().c_str());
//     chain.generateNewBlock(tmp.c_str(), tmp.size() + 1);
//     LOGGER->INFOF(3, "CHAINOPERATION APPEND:after insertion, current chain %s", chainToString().c_str());
//     rootBlockHash=chain.chain[chain.chain.size() - 1].getHash();

//     auto encodePair = encodeBlockChain(&chain);
//     char buffer[880];
//     string lastHash = estimatedHash.toString().substr(0, 16);

//     int packetNum = encodePair.first / 880;
//     if (encodePair.first % 880 != 0) {
//         packetNum++;
//     }
//     int remainingSize = encodePair.first;
//     char* ptr = encodePair.second.get();
//     for (int i = 0; i < packetNum; i++) {
//         int dataSize = remainingSize > 880 ? 880 : remainingSize;
//         remainingSize -= dataSize;
//         auto packet = make_shared<NdnInterest>();
//         packet->setName("/addr/broadcast/chain/" + lastHash + "/" + to_string(i) + "/" + to_string(packetNum));
//         packet->setNonce(rand());
//         memcpy(buffer, ptr, dataSize);
//         ptr += dataSize;
//         packet->setApplicationParameters(dataSize, buffer);
//         LOGGER->INFOF(3, "send out %s", packet->getName().c_str());
//         sendPacket(MacAddress("00:00:00:00:00:00"), packet);
//     }
//     }

// }

void NdnAddrAssignmentProtocol::generateBlock() {
    while (1) {
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        lock();
        if (blockBuffer.size() == 0) {
            unlock();
            continue;
        }

        unlock();
        // // BUG: there is a concurrency problem here
        // int interval = rand() % 2000;
        // this_thread::sleep_for(std::chrono::milliseconds(interval + routerID));
        // int pow = proveOfWork(1, chain.chain[chain.chain.size() - 1].getHash());
        int pow = 0;
        lock();

        // check whether blokcs for previous buffer was flushed by longest chain
        if (prevBuffer.size() != 0) {
            bool found = false;
            for (int i = 0; i < chain.chain.size(); i++) {
                if (estimatedHash == chain.chain[i].getHash()) {
                    found = true;
                    prevBuffer.clear();
                    break;
                }
            }
            if (!found) {
                for (string s : prevBuffer) {
                    blockBuffer.push_back(s);
                }
                prevBuffer.clear();
            }
        }

        stringstream ss;
        for (int i = 0; i < blockBuffer.size(); i++) {
            ss << blockBuffer[i];
            if (i != blockBuffer.size() - 1) {
                ss << ";";
            }
        }
        blockBuffer.clear();
        string assignmentInfo = ss.str();
        json info;
        info["assignmentInfo"] = ss.str();
        info["type"] = "assignment";
        info["signature"] = "";
        info["workproof"] = pow;

        info["routerID"] = routerID;
        info["certificate"] = "";
        auto p = addressPool->getPool();
        info["startaddr"] = p.first.toString();
        info["poolmask"] = p.second.toString();

        string tmp = info.dump();

        LOGGER->INFOF(3, "CHAINOPERATION APPEND: current chain %s", chainToString().c_str());
        chain.generateNewBlock(tmp.c_str(), tmp.size() + 1);
        LOGGER->INFOF(3, "CHAINOPERATION APPEND:after insertion, current chain %s", chainToString().c_str());

        prevBuffer = blockBuffer;
        estimatedHash = chain.chain[chain.chain.size() - 1].getHash();
        string lastHash = estimatedHash.toString().substr(0, 16);
        // send out new block
        auto encodePair = encodeBlockChain(&chain);
        auto newChain = decodeBlockChain(encodePair.second.get(), encodePair.first);



        int packetNum = encodePair.first / 880;
        if (encodePair.first % 880 != 0) {
            packetNum++;
        }
        int remainingSize = encodePair.first;
        char* ptr = encodePair.second.get();

        vector<shared_ptr<NdnInterest>>interests;
        for (int i = 0; i < packetNum; i++) {
            char buffer[880];
            int dataSize = remainingSize > 880 ? 880 : remainingSize;
            remainingSize -= dataSize;
            auto packet = make_shared<NdnInterest>();
            packet->setName("/addr/broadcast/chain/" + lastHash + "/" + to_string(i) + "/" + to_string(packetNum));
            packet->setNonce(rand());
            memcpy(buffer, ptr, dataSize);
            ptr += dataSize;
            packet->setApplicationParameters(dataSize, buffer);
            LOGGER->INFOF(3, "send out %s", packet->getName().c_str());
            sendPacket(MacAddress("00:00:00:00:00:00"), packet);
            interests.push_back(packet);
        }


        char b[100000];
        char* ptrr = b;
        int size = 0;
        for (int i = 0; i < packetNum; i++) {
            auto p = interests[i]->getApplicationParameters();
            memcpy(ptrr, p.second.get(), p.first);
            size += p.first;
            ptrr += p.first;
        }

        auto newChainr = decodeBlockChain(b, size);  
        unlock();
    }
}
