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

        // name:/routing/local/register/<from>/<to>/timestamp
        auto splits = split(interest->getName(), "/");
        if (splits.size() != 7) {
            LOGGER->ERRORF("invalid interest name %s", interest->getName().c_str());
            return;
        }
        RouterID sourceRouter = atoRID(splits[4].c_str());
        time_t timeStamp = atol(splits[6].c_str());
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
        protocol->addToRegisteredSon(registerPacket.root,sourceRouter);
        protocol->setLastRegistrationTime(registerPacket.root,sourceRouter,timeStamp);

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
        // name:/routing/local/register/<from>/<to>/timestamp
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        RegisterDataPack dataPack;
        auto tmp = data->getContent();
        dataPack.decode(tmp.second.get(), tmp.first);

        LOGGER->INFOF(2, "RegisterController::onReceiveData content: %s", dataPack.toString().c_str());

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

        // todo: add support for rch lsa
        if (dataPack.adjLsa != nullptr) {
            // 1. check whether the source is still marked as parent. if not, this packet should be regarded.
            auto adjLsa = dataPack.adjLsa;
            RouterID root = adjLsa->routerID;
            if (protocol->registeredParents.find(root) == protocol->registeredParents.end() ||
                protocol->registeredParents[root] == neighborObj->getRouterID()) {
                // parent is correct
                auto lsa=dataPack.adjLsa;
                bool newLsa=false,rebuild=false;
                auto existingLsa = protocol->database->findLsa(LinkStateType::ADJ, lsa->routerID);
                if(existingLsa == nullptr){
                    newLsa=true;
                    rebuild=true;
                    protocol->database->insertLsa(lsa);
                }else if (existingLsa->generateLSDigest() < lsa->generateLSDigest()){
                    rebuild=true;
                    protocol->database->insertLsa(lsa);
                }
                if(newLsa){
                    protocol->sendInfoToAll(lsa,interfaceIndex);
                }else if(rebuild){
                //if we have parents registered, we are supposed to send info to them
                    protocol->sendInfoToChildren(lsa);
                }
                if(rebuild){
                    //once a new lsa found, parents needed to be recalculated
                    protocol->registerParents();
                }
                //todo: if the routing table is changed, we are supposed to rebuild routing tables;

            } else {
                // parent is not correct
                LOGGER->WARNINGF("parent for root %d is not %d. Packet dropped", root, neighborObj->getRouterID());
            }
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}