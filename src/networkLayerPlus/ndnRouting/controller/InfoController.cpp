#include "InfoController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"

using namespace std;
void InfoController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceIndex);
            return;
        }
        // decide whether we need to send lsa interest.

        // 1.resolve the lsa type, source and sequence number
        //  /routing/hop/INFO/TYPE/routerID/seqNum
        auto splits = split(packet->getName(), "/");
        if (splits.size() != 7) {
            LOGGER->ERRORF("invalid info interest name %s", packet->getName().c_str());
            return;
        }
        LinkStateType lsType;
        if (splits[4] == "ADJ") {
            lsType = LinkStateType::ADJ;
        } else {
            lsType = LinkStateType::RCH;
        }

        RouterID routerID = atoRID(splits[5]);
        int32_t sequenceNum = atoi(splits[6].c_str());

        if (routerID == protocol->routerID) {
            return;
        }

        // search for the lsa
        auto existingLsa = protocol->database->findLsa(lsType, routerID);

        if (existingLsa == nullptr || sequenceNum > existingLsa->seqNum) {
            // we should request for this lsa.
            // check whether we have requested for this lsa
            if (!protocol->inBroadcastLsaPendingRequestList(lsType, routerID, sequenceNum)) {
                // record this into pending request
                LinkStateDigest digest;
                digest.linkStateType = lsType;
                digest.sequenceNum = sequenceNum;
                digest.routerID = routerID;
                protocol->broadcastLsaPendingRequestList.push_back(digest);
                // send out lsa request
                // usually we choose to send to the parents, but if parents is not found, a broadcast will be chosen instead
                auto interest = make_shared<NdnInterest>();
                string name = "/rt/hop/LSA/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string((unsigned long long)(digest.routerID)) +
                              "/" + to_string(digest.sequenceNum);
                interest->setNonce(rand());
                interest->setName(name);
                string logString = "sending out lsa interest " + name;

                auto registeredParent = protocol->minimumHopTree->getRegisteredParent(routerID);

                MacAddress targetMacAddress("ff:ff:ff:ff:ff:ff");
                int targetInterface = 0;
                RouterID targetID = 0;
                if (registeredParent.first) {
                    RouterID parent = registeredParent.second;
                    auto neighborObj = protocol->getNeighborByRouterID(parent);
                    if (neighborObj == nullptr) {
                        LOGGER->WARNINGF("neighbor not found %llu", parent);
                        return;
                    }
                    interest->setPreferedInterfaces({{neighborObj->getInterfaceID(), neighborObj->getMacAddress()}});
                    targetMacAddress = neighborObj->getMacAddress();
                    targetInterface = neighborObj->getInterfaceID();
                    targetID = neighborObj->getRouterID();
                    logString += " to " + to_string(parent);
                }
                LOGGER->INFOF(2, logString.c_str(), name.c_str());

                string timerName = "global_lsa_interest_timer " + to_string((unsigned long long)(digest.routerID)) + "_" + to_string(digest.linkStateType) +
                                   "_" + to_string(digest.sequenceNum);
                shared_ptr<int> retransmissionTime = make_shared<int>();
                *retransmissionTime = 0;
                IOC->getTimer()->startTimer(
                    timerName, NDNROUTING_DDRETRANSMISSION_INTERVAL * 1000, [this, interest, retransmissionTime, interfaceObj](string name) -> bool {
                        return protocol->getCrobJobHandler()->infoLsaExpireCronJob(retransmissionTime, interest, interfaceObj->getMacAddress(), name);
                    });

                // protocol->unlock();
                protocol->sendPacket(interfaceObj->getMacAddress(), interest);
                // protocol->lock();
            }
        }

        // decide to whom we should send this packet

        vector<std::pair<int, MacAddress>> interfaces;
        for (auto son : protocol->minimumHopTree->getRegisteredSons(routerID)) {
            auto neighborObj = protocol->getNeighborByRouterID(son);
            // it is possible that we haven't established this neighbor yet
            if (neighborObj == nullptr) {
                LOGGER->WARNINGF("we haven't discovered this neighbor %d yet when forwarding info %s", son, packet->getName().c_str());
                continue;
            }
            interfaces.push_back({neighborObj->getInterfaceID(), neighborObj->getMacAddress()});
        }
        LOGGER->INFOF(2, "forwarding INFO %s to %s", packet->getName().c_str(), intMacAddressVectorToString(interfaces).c_str());
        if (interfaces.size() != 0) {
            packet->setPreferedInterfaces(interfaces);
            // protocol->unlock();
            protocol->sendPacket(interfaceObj->getMacAddress(), packet);
            // protocol->lock();
        }
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void InfoController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}