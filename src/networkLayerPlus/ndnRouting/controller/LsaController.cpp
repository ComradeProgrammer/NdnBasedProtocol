#include "LsaController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"

using namespace std;
void LsaController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceIndex);
            return;
        }

        auto splits = split(interest->getName(), "/");
        if (splits.size() != 7) {
            LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveLsaInterest: invalid name %s", interest->getName().c_str());
            return;
        }
        // resolve the packet
        // "/routing/local/LSA/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string(digest.routerID) + "/" + to_string(digest.sequenceNum);
        LinkStateType lsType;
        if (splits[4] == "ADJ") {
            lsType = LinkStateType::ADJ;
        } else {
            lsType = LinkStateType::RCH;
        }

        RouterID routerID = atoRID(splits[5]);
        int32_t sequenceNum = atoi(splits[6].c_str());

        auto neighborObj = interfaceObj->getNeighborByMac(sourceMac);
        if (neighborObj == nullptr) {
            LOGGER->WARNING("neighbor not found");
            return;
        }
        // depend on whether this lsa interest is local
        if (splits[2] == "local") {
            shared_ptr<LsaDataPack> lsa = nullptr;
            if (splits[4] == "ADJ") {
                lsa = protocol->database->findLsa(ADJ, routerID);
            } else if (splits[4] == "RCH") {
                lsa = protocol->database->findLsa(RCH, routerID);
            }
            if (lsa == nullptr) {
                LOGGER->ERRORF("no asscoiated lsa found %d from %s", routerID, interest->getName().c_str());
                return;
            }
            auto newPacket = make_shared<NdnData>();
            newPacket->setName(interest->getName());
            auto encoded = lsa->encode();
            newPacket->setContent(encoded.first, encoded.second.get());
            newPacket->setPreferedInterfaces({{interfaceIndex, sourceMac}});

            LOGGER->INFOF(2, "sending local Lsa %s to router %d content %s", newPacket->getName().c_str(), neighborObj->getRouterID(), lsa->toString().c_str());
            // protocol->unlock();
            protocol->sendPacket(interfaceObj->getMacAddress(), newPacket);
            // protocol->lock();
        } else if (splits[2] == "hop") {
            shared_ptr<LsaDataPack> lsa = nullptr;
            if (splits[4] == "ADJ") {
                lsa = protocol->database->findLsa(ADJ, routerID);
            } else if (splits[4] == "RCH") {
                lsa = protocol->database->findLsa(RCH, routerID);
            }

            if (lsa != nullptr && lsa->seqNum == sequenceNum) {
                // we have that lsa, just send it back
                auto newPacket = make_shared<NdnData>();
                newPacket->setName(interest->getName());
                auto encoded = lsa->encode();
                newPacket->setContent(encoded.first, encoded.second.get());
                LOGGER->INFOF(2, "sending hop Lsa %s content %s", newPacket->getName().c_str(), lsa->toString().c_str());

                // protocol->unlock();
                protocol->sendPacket(interfaceObj->getMacAddress(), newPacket);
                // protocol->lock();
            } else if (lsa != nullptr && lsa->seqNum > sequenceNum) {
                // we have newer lsa, so send back a empty data, indicating that requested lsa has expired
                auto newPacket = make_shared<NdnData>();
                newPacket->setName(interest->getName());
                LOGGER->INFOF(2, "sending hop Lsa %s",newPacket->getName().c_str());
                protocol->sendPacket(interfaceObj->getMacAddress(), newPacket);
            } else {
                // no lsa found, decide where we should send it out
                auto registeredParent = protocol->minimumHopTree->getRegisteredParent(routerID);
                if (registeredParent.first) {
                    RouterID target = registeredParent.second;
                    auto neighborObj = protocol->getNeighborByRouterID(target);
                    if (neighborObj == nullptr) {
                        LOGGER->WARNINGF("neighbor not found %d", target);
                        return;
                    }
                    interest->setPreferedInterfaces({{neighborObj->getInterfaceID(), neighborObj->getMacAddress()}});

                    LOGGER->INFOF(2, "sending lsa interest %s to %d", interest->getName().c_str(), target);
                    // protocol->unlock();
                    protocol->sendPacket(neighborObj->getBelongingInterface()->getMacAddress(), interest);
                    // protocol->lock();
                } else {
                    LOGGER->INFOF(2, "no proper target found for lsa interest %s ", interest->getName().c_str());
                }
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

void LsaController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data) {
    try {
        bool rebuild = false, newLsa = false;

        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
       

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("source interface %d not found", interfaceIndex);
            return;
        }
        auto splits = split(data->getName(), "/");
        if (splits.size() != 7) {
            LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveLsaData: invalid name %s", data->getName().c_str());
            return;
        }
        // resolve the packet
        // "/routing/local/LSA/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string(digest.routerID) + "/" + to_string(digest.sequenceNum);
        LinkStateType lsType;
        if (splits[4] == "ADJ") {
            lsType = LinkStateType::ADJ;
        } else {
            lsType = LinkStateType::RCH;
        }

        RouterID routerID = atoRID(splits[5]);
        int32_t sequenceNum = atoi(splits[6].c_str());

        string timerName = "global_lsa_interest_timer " + to_string((unsigned long long)routerID) + "_" + to_string(lsType) + "_" + to_string(sequenceNum);
        IOC->getTimer()->cancelTimer(timerName);

        shared_ptr<LsaDataPack> lsa = make_shared<LsaDataPack>();
        auto tmp = data->getContent();
        if(tmp.first==0){
            LOGGER->INFOF(2,"empty lsa data %s",data->getName().c_str());
            protocol->removeFromBroadcastLsaPendingRequestList(lsType,routerID, sequenceNum);

            return;
        }
        lsa->decode(tmp.second.get(), tmp.first);


        switch (lsa->lsType) {
            case LinkStateType::ADJ: {
                auto existingLsa = protocol->database->findLsa(LinkStateType::ADJ, lsa->routerID);
                if (existingLsa == nullptr) {
                    newLsa = true;
                    rebuild = true;
                    protocol->database->insertLsa(lsa);
                } else if (existingLsa->generateLSDigest() < lsa->generateLSDigest()) {
                    rebuild = true;
                    protocol->database->insertLsa(lsa);
                }

                if (newLsa) {
                    // for those lsas whose source was never recorded before, we are supposed to send broadcasts INFO
                    protocol->sendInfoToAll(lsa, interfaceIndex);
                } else {
                    // if we have parents registered, we are supposed to send info to them
                    protocol->sendInfoToChildren(lsa);
                }

                break;
            }
            case LinkStateType::RCH: {
                auto existingLsa = protocol->database->findLsa(LinkStateType::RCH, lsa->routerID);
                if (existingLsa == nullptr) {
                    newLsa = true;
                    rebuild = true;
                    protocol->database->insertLsa(lsa);
                } else if (existingLsa->generateLSDigest() < lsa->generateLSDigest()) {
                    rebuild = true;
                    protocol->database->insertLsa(lsa);
                }

                if (newLsa) {
                    // for those lsas whose source was never recorded before, we are supposed to send broadcasts INFO
                    protocol->sendInfoToAll(lsa, interfaceIndex);
                } else {
                    // if we have parents registered, we are supposed to send info to them
                    protocol->sendInfoToChildren(lsa);
                }

                break;
            }
        }
        if (splits[2] == "local") {
            // local LSA
            auto interfaceObj = protocol->interfaces[interfaceIndex];
            if (interfaceObj == nullptr) {
                LOGGER->ERRORF("interface %d not found", interfaceIndex);
                return;
            }
            for (auto neighbor : interfaceObj->getNeighbors()) {
                if (neighbor.second->getState() < NeighborStateType::EXCHANGE) {
                    continue;
                }
                // remove from local lsa pending list
                neighbor.second->cancelLsaInterestRequest(lsa->generateLSDigest());
                // check whether exchanging stage is over
                if (neighbor.second->isLocalLsaPendingRequestListEmpty()) {
                    neighbor.second->processEvent(NeighborEventType::LOADING_DONE);
                }
            }
        } else {
            protocol->removeFromBroadcastLsaPendingRequestList(lsa->lsType, lsa->routerID, lsa->seqNum);
        }
        if (rebuild) {
            protocol->registerParents();
            protocol->rebuildRoutingTable();
        }
        // todo: for those lsas whose source was never recorded before, we are supposed to send brodadcast INFO

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}