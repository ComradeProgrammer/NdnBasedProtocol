#include "RegisterController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"

using namespace std;

void RegisterController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceIndex);
            return;
        }

        // name:/routing/local/register/"+to_string(routerID)+"/"+ to_string(timestamp)
        auto splits = split(interest->getName(), "/");
        if (splits.size() != 6) {
            LOGGER->ERRORF("invalid interest name %s", interest->getName().c_str());
            return;
        }
        RouterID sourceRouter = atol(splits[4].c_str());
        time_t timeStamp = atol(splits[5].c_str());
        // decode the packet
        RegisterInterestPack registerPacket;
        auto contentPair = interest->getApplicationParameters();
        registerPacket.decode(contentPair.second.get(), contentPair.first);
        LOGGER->INFOF(2, "RegisterController::onReceiveInterest %s", registerPacket.toString().c_str());
        // check whether this packet is latest packet;
        long oldTimeStamp = protocol->getLastRegistrationTime(registerPacket.root, sourceRouter);

        RegisterDataPack dataPack;
        // prepare the response
        // send the empty response if this request is outdated
        if (timeStamp > oldTimeStamp) {
            // check the sequence number  of lsa for root
            auto existingLsa = protocol->database->findLsa(LinkStateType::ADJ, registerPacket.root);
            if (existingLsa != nullptr && existingLsa->seqNum > registerPacket.sequenceNum) {
                dataPack.adjLsa = existingLsa;
            }
        }
        // send the response
        auto data = make_shared<NdnData>();
        data->setName(interest->getName());
        auto encoded = dataPack.encode();
        data->setContent(encoded.first, encoded.second.get());
        data->setPreferedInterfaces({{interfaceIndex, sourceMac}});
        LOGGER->INFOF(2, "sening register data %s to router %d,content %s", data->getName().c_str(), sourceRouter, dataPack.toString().c_str());

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
void RegisterController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        RegisterDataPack dataPack;
        auto tmp = data->getContent();
        dataPack.decode(tmp.second.get(), tmp.first);

        LOGGER->INFOF(2, "RegisterController::onReceiveData content: %s", dataPack.toString().c_str());

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}