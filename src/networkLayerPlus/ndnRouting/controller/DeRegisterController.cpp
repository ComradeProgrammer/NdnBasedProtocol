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

        // check whether this packet is latest packet;
        long oldTimeStamp = protocol->minimumHopTree->getLastRegistrationTime(registerPacket.root, sourceRouter);

        if (timeStamp > oldTimeStamp) {
            protocol->minimumHopTree->deleteFromRegisteredSon(registerPacket.root, sourceRouter);
            protocol->minimumHopTree->setLastRegistrationTime(registerPacket.root, sourceRouter, timeStamp);
        }

        auto data = make_shared<NdnData>();
        data->setName(interest->getName());
        data->setPreferedInterfaces({{interfaceIndex, sourceMac}});
        LOGGER->INFOF(2, "sening deregister data %s to router %d", data->getName().c_str(), sourceRouter);

        protocol->unlock();
        protocol->sendPacket(interfaceObj->getMacAddress(), data);
        protocol->lock();

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void DeRegisterController::onReceiveData(int interfaceIndex, MacAddress sourceMac, shared_ptr<NdnData> packet) {
    string timerName = "deregister_" +packet->getName();
    IOC->getTimer()->cancelTimer(timerName);
}