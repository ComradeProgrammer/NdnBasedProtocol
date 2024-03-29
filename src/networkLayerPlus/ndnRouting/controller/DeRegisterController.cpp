#include "DeRegisterController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
using namespace std;
void DeRegisterController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceIndex);
            return;
        }

        // name:/routing/local/deregister/<from>/<to>/timestamp
        auto splits = split(interest->getName(), "/");
        if (splits.size() != 7) {
            LOGGER->ERRORF("invalid interest name %s", interest->getName().c_str());
            return;
        }
        RouterID sourceRouter = atoRID(splits[4].c_str());
        time_t timeStamp = atol(splits[6].c_str());

        DeRegisterInterestPack registerPacket;
        auto contentPair = interest->getApplicationParameters();
        registerPacket.decode(contentPair.second.get(), contentPair.first);
        LOGGER->INFOF(2, "DeRegisterController::onReceiveInterest %s", registerPacket.toString().c_str());

        AuditEventPacketIn event(getCurrentTime(), interfaceIndex, sourceMac, sourceRouter, AuditEventInterface::INTEREST,
                                 AuditEventInterface::DEREGISTER_PACKET, interest->getName(), registerPacket.marshal());
        IOC->getAuditRecorder()->insertAuditLog(event);
        NdnRoutingAclData acldata;
        acldata.interfaceIndex = interfaceIndex;
        acldata.packetKind = PacketKind::DEREGISTER;
        acldata.packetType = PacketType::INTEREST;
        acldata.packetName = interest->getName();
        acldata.sourceMacAddress = sourceMac;
        acldata.sourceRouterID = sourceRouter;
        IOC->getNdnRoutingAcl()->match(&acldata);

        // check whether this packet is latest packet;
        long oldTimeStamp = protocol->minimumHopTree->getLastRegistrationTime(registerPacket.root, sourceRouter);

        if (timeStamp > oldTimeStamp) {
            protocol->minimumHopTree->deleteFromRegisteredSon(registerPacket.root, sourceRouter);
            protocol->minimumHopTree->setLastRegistrationTime(registerPacket.root, sourceRouter, timeStamp);

            AuditEventRegister event(getCurrentTime(), sourceRouter, registerPacket.root, protocol->getRouterID(), AuditEventRegister::DEREGISTER);
            IOC->getAuditRecorder()->insertAuditLog(event);
        }

        auto data = make_shared<NdnData>();
        data->setName(interest->getName());
        data->setPreferedInterfaces({{interfaceIndex, sourceMac}});
        LOGGER->INFOF(2, "sening deregister data %s to router %llu", data->getName().c_str(), sourceRouter);

        AuditEventPacketOut event2(getCurrentTime(), interfaceIndex, sourceMac, sourceRouter, AuditEventInterface::DATA, AuditEventInterface::DEREGISTER_PACKET,
                                   data->getName(), nlohmann::json{});
        IOC->getAuditRecorder()->insertAuditLog(event2);

        // protocol->unlock();
        protocol->sendPacket(interfaceObj->getMacAddress(), data);
        // protocol->lock();

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void DeRegisterController::onReceiveData(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnData> packet) {
    lock_guard<mutex> lockFunction(*(protocol->mutexLock));
    auto interfaceObj = protocol->interfaces[interfaceIndex];
    if (interfaceObj == nullptr) {
        LOGGER->ERRORF("interface %d not found", interfaceIndex);
        return;
    }
    auto neighborObj = interfaceObj->getNeighborByMac(sourceMac);
    if (neighborObj == nullptr) {
        LOGGER->WARNING("neighbor not found");
        return;
    }

    RouterID sourceRouter = neighborObj->getRouterID();
    string timerName = "deregister_" + packet->getName();
    AuditEventPacketIn event(getCurrentTime(), interfaceIndex, sourceMac, sourceRouter, AuditEventInterface::DATA, AuditEventInterface::REGISTER_PACKET,
                             packet->getName(), nlohmann::json{});
    IOC->getAuditRecorder()->insertAuditLog(event);

    NdnRoutingAclData acldata;
    acldata.interfaceIndex = interfaceIndex;
    acldata.packetKind = PacketKind::DEREGISTER;
    acldata.packetType = PacketType::DATA;
    acldata.packetName = packet->getName();
    acldata.sourceMacAddress = sourceMac;
    acldata.sourceRouterID = sourceRouter;
    IOC->getNdnRoutingAcl()->match(&acldata);
    IOC->getTimer()->cancelTimer(timerName);
}