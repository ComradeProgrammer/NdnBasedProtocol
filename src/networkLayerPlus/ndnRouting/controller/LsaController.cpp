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
        LsaInterestPack lsaInterestPack;
        auto dataPair = interest->getApplicationParameters();
        lsaInterestPack.decode(dataPair.second.get(), dataPair.first);

        // depend on whether this lsa interest is local
        if (splits[2] == "local") {
            shared_ptr<LsaDataPack> lsa = nullptr;
            if (splits[4] == "ADJ") {
                lsa = protocol->database->findLsa(ADJ, lsaInterestPack.routerID);
            } else if (splits[4] == "RCH") {
                lsa = protocol->database->findLsa(RCH, lsaInterestPack.routerID);
            }
            if (lsa == nullptr) {
                LOGGER->ERRORF("no asscoiated lsa found %d from %s",lsaInterestPack.routerID,interest->getName().c_str());
                return;
            }
            auto newPacket = make_shared<NdnData>();
            newPacket->setName(interest->getName());
            auto encoded = lsa->encode();
            newPacket->setContent(encoded.first, encoded.second.get());
            newPacket->setPreferedInterfaces({{interfaceIndex, sourceMac}});
            protocol->unlock();
            protocol->sendPacket(interfaceObj->getMacAddress(),newPacket);
            protocol->lock();
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
        bool rebuild = false;

        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        shared_ptr<LsaDataPack> lsa = make_shared<LsaDataPack>();
        auto tmp = data->getContent();
        lsa->decode(tmp.second.get(), tmp.first);

        switch (lsa->lsType) {
            case LinkStateType::ADJ: {
                auto existingLsa = protocol->database->findLsa(LinkStateType::ADJ, lsa->routerID);
                if (existingLsa == nullptr || (existingLsa->generateLSDigest() < lsa->generateLSDigest())) {
                    protocol->database->insertLsa(lsa);
                    rebuild = true;
                }
                break;
            }
            case LinkStateType::RCH: {
                auto existingLsa = protocol->database->findLsa(LinkStateType::RCH, lsa->routerID);
                if (existingLsa == nullptr || (existingLsa->generateLSDigest() < lsa->generateLSDigest())) {
                    protocol->database->insertLsa(lsa);
                    rebuild = true;
                }
                break;
            }
        }
        auto splits = split(data->getName(), "/");
        if (splits[2] == "local") {
            auto interfaceObj = protocol->interfaces[interfaceIndex];
            if (interfaceObj == nullptr) {
                LOGGER->ERRORF("interface %d not found", interfaceIndex);
                return;
            }
            for (auto neighbor : interfaceObj->getNeighbors()) {
                if (neighbor.second->getState() < NeighborStateType::EXCHANGE) {
                    continue;
                }
                neighbor.second->cancelLsaInterestRequest(lsa->generateLSDigest());
                if (neighbor.second->isLocalLsaPendingRequestListEmpty()) {
                    neighbor.second->processEvent(NeighborEventType::LOADING_DONE);
                }
            }
        } else {
            // todo handle broadcast lsa
        }
        if (rebuild) {
            //sendInfoInterestDueToNeighbor
            //database.rebuildRoutingTable();
        }
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}