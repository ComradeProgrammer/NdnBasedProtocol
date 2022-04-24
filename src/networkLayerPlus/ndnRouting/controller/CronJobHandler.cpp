#include "CronJobHandler.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/dataPack/HelloInterestPack.h"
using namespace std;
void CronJobHandler::sendingHelloMessageCronJob(int interfaceIndex) {
    try {
        protocol->lock();

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        HelloInterestPack helloPack;
        helloPack.routerId = protocol->getRouterID();
        helloPack.interfaceIP = interfaceObj->getIpv4Address();
        helloPack.networkMask = interfaceObj->getIpv4Mask();
        helloPack.helloInterval = NDNROUTING_HELLOINTERVAL;
        helloPack.routerDeadInterval = NDNROUTING_ROUTERDEADINTERVAL;
        for (auto intf : protocol->interfaces) {
            for (auto neighbor : intf.second->neighbors) {
                helloPack.neighbor.push_back(neighbor.second->getIpv4Address());
            }
        }

        // todo: take neighbor situation into consideration
        helloPack.publicKey = new char[PUBLIC_KEY_LENGTH];
        memcpy(helloPack.publicKey, protocol->getPublicKey().c_str(), PUBLIC_KEY_LENGTH);

        auto hash = protocol->database->databaseHash();
        for (int i = 0; i < 16; i++) {
            helloPack.databaseHash[i] = hash.first[i];
        }
        helloPack.signSignature(protocol->privateKey);

        auto encodePair = helloPack.encode();
        auto packet = make_shared<NdnInterest>();

        shared_ptr<SymmetricCipher> encryptor = make_shared<Aes>();
        string key = protocol->getPassword();
        encryptor->setKey(key.c_str(), key.size());
        auto encryptedPair = encryptor->encrypt(encodePair.second.get(), encodePair.first);

        packet->setName("/routing/local/hello");
        packet->setNonce(rand());
        packet->setApplicationParameters(encryptedPair.second, (const char*)encryptedPair.first.get());
        packet->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});

        AuditEventPacketOut event(getCurrentTime(), interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff"), 0, AuditEventInterface::INTEREST,
                                  AuditEventInterface::HELLO_PACKET, packet->getName(), helloPack.marshal());
        IOC->getAuditRecorder()->insertAuditLog(event);

        protocol->sendPacket(interfaceObj->getMacAddress(), packet);
        protocol->unlock();
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void CronJobHandler::neighborInactivityCronJob(NdnRoutingNeighbor* neighbor) {
    try {
        protocol->lock();
        neighbor->processEvent(NeighborEventType::INACTIVITY_TIMER);
        protocol->unlock();
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

bool CronJobHandler::ddInterestExpireCronJob(shared_ptr<int> retransmissionTime, std::shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        packet->setNonce(rand());
        // NO NEED to lock, because there is nothing to lock.
        LOGGER->WARNINGF("CronJobHandler::ddInterestExpireCronJob: retransmissing info %s", timerName.c_str());
        protocol->sendPacket(sourceMac, packet);
        (*(retransmissionTime))++;
        if (*(retransmissionTime) >= 3) {
            LOGGER->WARNINGF("CronJobHandler::ddInterestExpireCronJob: maximum retry time exceeded  %s", timerName.c_str());
            return false;
        }
        return true;
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

bool CronJobHandler::localLsaExpireCronJob(shared_ptr<int> retransmissionTime, shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        packet->setNonce(rand());

        LOGGER->WARNINGF("CronJobHandler::localLsaExpireCronJob: retransmissing info %s", timerName.c_str());
        protocol->sendPacket(sourceMac, packet);
        (*(retransmissionTime))++;
        if (*(retransmissionTime) >= 3) {
            LOGGER->WARNINGF("CronJobHandler::localLsaExpireCronJob: maximum retry time exceeded  %s", timerName.c_str());
            return false;
        }
        return true;
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

bool CronJobHandler::infoLsaExpireCronJob(shared_ptr<int> retransmissionTime, shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        packet->setNonce(rand());

        LOGGER->WARNINGF("CronJobHandler::infoLsaExpireCronJob: retransmissing info %s", timerName.c_str());
        protocol->sendPacket(sourceMac, packet);
        (*(retransmissionTime))++;
        if (*(retransmissionTime) >= 3) {
            LOGGER->WARNINGF("CronJobHandler::infoLsaExpireCronJob: maximum retry time exceeded  %s", timerName.c_str());
            auto splits = split(packet->getName(), "/");
            LinkStateType lsType;
            if (splits[4] == "ADJ") {
                lsType = LinkStateType::ADJ;
            } else {
                lsType = LinkStateType::RCH;
            }

            RouterID routerID = atoRID(splits[5]);
            int32_t sequenceNum = atoi(splits[6].c_str());

            protocol->removeFromBroadcastLsaPendingRequestList(lsType, routerID, sequenceNum);
            return false;
        }
        return true;
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

bool CronJobHandler::registerExpireCronJob(shared_ptr<int> retransmissionTime, shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        packet->setNonce(rand());

        LOGGER->WARNINGF("CronJobHandler::registerExpireCronJob: retransmissing info %s", timerName.c_str());
        protocol->sendPacket(sourceMac, packet);
        (*(retransmissionTime))++;
        if (*(retransmissionTime) >= 3) {
            LOGGER->WARNINGF("CronJobHandler::registerExpireCronJob: maximum retry time exceeded  %s", timerName.c_str());
            return false;
        }
        return true;
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
bool CronJobHandler::deRegisterExpireCronJob(shared_ptr<int> retransmissionTime, shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        packet->setNonce(rand());

        LOGGER->WARNINGF("CronJobHandler::deRegisterExpireCronJob: retransmissing info %s", timerName.c_str());
        protocol->sendPacket(sourceMac, packet);
        (*(retransmissionTime))++;
        if (*(retransmissionTime) >= 3) {
            LOGGER->WARNINGF("CronJobHandler::deRegisterExpireCronJob: maximum retry time exceeded  %s", timerName.c_str());
            return false;
        }
        return true;
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}