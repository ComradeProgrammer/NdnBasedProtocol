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
        long oldTimeStamp = protocol->minimumHopTree->getLastRegistrationTime(registerPacket.root, sourceRouter);

        RegisterDataPack dataPack;
        // prepare the response
        // send the empty response if this request is outdated
        if (timeStamp > oldTimeStamp) {
            // check the sequence number  of lsa for root
            auto existingLsa = protocol->database->findLsa(LinkStateType::ADJ, registerPacket.root);
            if (existingLsa != nullptr && existingLsa->seqNum > registerPacket.adjSequenceNum) {
                dataPack.adjLsa = existingLsa;
            }
            protocol->minimumHopTree->addToRegisteredSon(registerPacket.root, sourceRouter);
            protocol->minimumHopTree->setLastRegistrationTime(registerPacket.root, sourceRouter, timeStamp);
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

        string timerName = "register_" +data->getName();
        IOC->getTimer()->cancelTimer(timerName);

        bool rebuild = false;

        if (dataPack.adjLsa != nullptr) {
            // 1. check whether the source is still marked as parent. if not, this packet should be disgarded.
            auto adjLsa = dataPack.adjLsa;
            RouterID root = adjLsa->routerID;

            auto registeredParent=protocol->minimumHopTree->getRegisteredParent(root);
            if (registeredParent.first && registeredParent.second== neighborObj->getRouterID()) {
                // parent is correct
                bool newLsa = false;
                auto existingLsa = protocol->database->findLsa(LinkStateType::ADJ, adjLsa->routerID);
                if (existingLsa == nullptr) {
                    newLsa = true;
                    rebuild = true;
                    protocol->database->insertLsa(adjLsa);
                } else if (existingLsa->generateLSDigest() < adjLsa->generateLSDigest()) {
                    rebuild = true;
                    protocol->database->insertLsa(adjLsa);
                }
                if (newLsa) {
                    //在这种情况下需要这样做的原因是这样： 1-2 3-4， 2-3连接之后1不会知道4的存在所以不会请求4
                    protocol->sendInfoToAll(adjLsa, interfaceIndex);
                    protocol->rebuildRoutingTable();

                } else if (rebuild) {
                    // if we have parents registered, we are supposed to send info to them
                    protocol->sendInfoToChildren(adjLsa);
                    protocol->rebuildRoutingTable();
                }

            } else {
                // parent is not correct
                LOGGER->WARNINGF("parent for root %d is not %d. Packet dropped", root, neighborObj->getRouterID());
            }
        }

        if (dataPack.rchLsa != nullptr) {
            auto rchLsa = dataPack.rchLsa;
            RouterID root = rchLsa->routerID;
            auto registeredParent=protocol->minimumHopTree->getRegisteredParent(root);
            if (registeredParent.first && registeredParent.second== neighborObj->getRouterID()) {
                bool newLsa = false;
                auto existingLsa = protocol->database->findLsa(LinkStateType::RCH, rchLsa->routerID);
                if (existingLsa == nullptr) {
                    newLsa = true;
                    rebuild = true;
                    protocol->database->insertLsa(rchLsa);
                } else if (existingLsa->generateLSDigest() < rchLsa->generateLSDigest()) {
                    rebuild = true;
                    protocol->database->insertLsa(rchLsa);
                }
                if (newLsa) {
                    //在这种情况下需要这样做的原因是这样： 1-2 3-4， 2-3连接之后1不会知道4的存在所以不会请求4
                    protocol->sendInfoToAll(rchLsa, interfaceIndex);
                } else if (rebuild) {
                    // if we have parents registered, we are supposed to send info to them
                    protocol->sendInfoToChildren(rchLsa);
                }
            }
            
        }

        if (rebuild) {
            // once a new lsa found, parents needed to be recalculated
            protocol->registerParents();
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}