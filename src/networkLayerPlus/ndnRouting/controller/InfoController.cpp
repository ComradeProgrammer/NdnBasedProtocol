#include "InfoController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"

using namespace std;
void InfoController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) {
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
            string name =
                "/routing/hop/LSA/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string(digest.routerID) + "/" + to_string(digest.sequenceNum);
            interest->setNonce(rand());
            interest->setName(name);
            string logString = "sending out lsa interest " + name;
            if (protocol->registeredParents.find(routerID) != protocol->registeredParents.end()) {
                RouterID parent = protocol->registeredParents[routerID];
                auto neighborObj = protocol->getNeighborByRouterID(parent);
                interest->setPreferedInterfaces({{neighborObj->getInterfaceID(), neighborObj->getMacAddress()}});
                logString += " to " + to_string(parent);
            }
            LOGGER->INFOF(2, logString.c_str(), name.c_str());
            protocol->sendPacket(interfaceObj->getMacAddress(), interest);
        }
    }

    // decide to whom we should send this packet
    if (protocol->registeredSons.find(routerID) != protocol->registeredSons.end()) {
        vector<std::pair<int, MacAddress>> interfaces;
        for (auto son : protocol->registeredSons[routerID]) {
            auto neighborObj = protocol->getNeighborByRouterID(son);
            interfaces.push_back({neighborObj->getInterfaceID(), neighborObj->getMacAddress()});
        }
        LOGGER->INFOF(2, "forwarding INFO %s to %s", packet->getName().c_str(), intMacAddressVectorToString(interfaces).c_str());
        if (interfaces.size() != 0) {
            packet->setPreferedInterfaces(interfaces);
            protocol->sendPacket(interfaceObj->getMacAddress(), packet);
        }
    }
}

void InfoController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}