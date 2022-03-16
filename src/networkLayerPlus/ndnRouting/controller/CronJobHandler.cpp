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
        for (auto neighbor : interfaceObj->neighbors) {
            helloPack.neighbor.push_back(neighbor.second->getIpv4Address());
        }

        auto hash=protocol->database->databaseHash();
        for(int i=0;i<16;i++){
            helloPack.databaseHash[i]=hash[i];
        }

        auto encodePair = helloPack.encode();
        auto packet = make_shared<NdnInterest>();
        packet->setName("/routing/local/hello");
        packet->setNonce(rand());
        packet->setApplicationParameters(encodePair.first, encodePair.second.get());
        packet->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});

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

bool CronJobHandler::registerExpireCronJob(shared_ptr<int> retransmissionTime, shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName){
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
bool CronJobHandler::deRegisterExpireCronJob(shared_ptr<int> retransmissionTime, shared_ptr<NdnInterest> packet, MacAddress sourceMac, string timerName){
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