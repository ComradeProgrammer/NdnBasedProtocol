#include "DDController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
using namespace std;
void DDController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
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

        // if we are in init state, just turn to the exchange state
        if (neighborObj->getState() == NeighborStateType::INIT) {
            neighborObj->processEvent(NeighborEventType::TWOWAY_RECEIVED);
        }

        // we allow asking for retransmission, but we don't allow the request of lsa earlier than that
        auto splits = split(interest->getName(), "/");
        if (splits.size() < 6) {
            LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveDDInterset invalid dd interest name %s", interest->getName().c_str());

            return;
        }
        int requestedIndex = atoi(splits[5].c_str());

        bool ok = neighborObj->sendDDData(requestedIndex, interest->getName());
        if (!ok) {
            neighborObj->dragPeerToInit();
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void DDController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data) {
    try {
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
        };

        if (neighborObj->getState() != NeighborStateType::EXCHANGE) {
            LOGGER->WARNINGF("NdnRoutingNeighbor::onReceiveDDData: dd packet dropped due to incorrect state. packet %s, current state %s",
                             data->toString().c_str(), getNameForNeighborState(neighborObj->getState()).c_str());
        }

        auto splits = split(data->getName(), "/");
        if (splits.size() < 6) {
            LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveDDData invalid dd data name %s", data->getName().c_str());
            return;
        }
        // check the index, drop it if we have received this
        int offeredIndex = atoi(splits[5].c_str());
        if (offeredIndex < neighborObj->getReceivingIndex()) {
            // duplicated packet
            LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveDDData dropped due to expired index %s", data->getName().c_str());
            return;
        } else if (offeredIndex > neighborObj->getReceivingIndex()) {
            LOGGER->ERRORF("NdnRoutingNeighbor::onReceiveDDData dropped due to exceeding index %s", data->getName().c_str());
            neighborObj->dragPeerToInit();
            return;
        }
        // remove the timer
        string timerName = string("dd_interest_timer") + to_string(interfaceObj->getInterfaceID()) + "_" +
                           to_string((unsigned long long)(neighborObj->getRouterID())) + "_" + to_string(offeredIndex);
        neighborObj->deleteTimer(timerName);

        // resolve the datapack
        DDDataPack dataPack;
        auto contentPair = data->getContent();

        dataPack.decode(contentPair.second.get(), contentPair.first);
        LOGGER->INFOF(2, "NdnRoutingNeighbor::onReceiveDDData: dataPack content is %s", dataPack.toString().c_str());

        // check every digest listed in data
        for (int i = 0; i < dataPack.ls.size(); i++) {
            if (dataPack.ls[i].linkStateType != ADJ && dataPack.ls[i].linkStateType != RCH) {
                LOGGER->ERROR("NdnRoutingNeighbor::onReceiveDDData dropped due to invalid linkstate digest type");
                return;
            }
            auto existingLsa = protocol->database->findLsa(dataPack.ls[i].linkStateType, dataPack.ls[i].routerID);
            if (existingLsa == nullptr) {
                // if we haven't got the lsa yet, request for it
                neighborObj->sendLocalLsaInterest(dataPack.ls[i]);
            } else if (existingLsa->generateLSDigest() < dataPack.ls[i] &&
                       !protocol->inBroadcastLsaPendingRequestList(dataPack.ls[i].linkStateType, dataPack.ls[i].routerID, dataPack.ls[i].sequenceNum)) {
                neighborObj->sendLocalLsaInterest(dataPack.ls[i]);
            }
        }

        neighborObj->receivingIndexIncrement();
        if (neighborObj->getReceivingIndex() == dataPack.numberOfDDPackets || 0 == dataPack.numberOfDDPackets) {
            neighborObj->processEvent(NeighborEventType::EXCHANGE_DONE);
        } else {
            neighborObj->sendDDInterest();
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
