#include "AddrRequestController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"

using namespace std;
void AddrRequestController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        LOGGER->INFOF(3, "AddrRequestController::onReceiveInterest AddrRequest received %s", interest->getName().c_str());
        // string name="/addr/broadcast/req/"+to_string(protocol->getRouterID())+"/"+to_string(interfaceID)+"/"+to_string(requestSize);

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceIndex);
            return;
        }

        auto splits = split(interest->getName(), "/");
        RouterID router = atoRID(splits[4]);
        int interfaceID = atoi(splits[5].c_str());
        if (splits[2] == "broadcast") {
            if (!protocol->getIsRoot()) {
                // is not root, ignore and let the packet  be transmitted
                return;
            }
            int num = atoi(splits[6].c_str());
            AddrRequestData addrReqData;
            // look up existing assignment record
            bool found = false;
            for (auto p : protocol->rootAssignment) {
                if (p.second.routerID == router && p.second.interfaceIndex == interfaceID) {
                    found = true;
                    addrReqData = p.second.data;
                    break;
                }
            }
            // assign a new one
            if (!found) {
                auto res = protocol->getAddressPool()->allocateAddress(num);
                if (res.first == Ipv4Address("0.0.0.0")) {
                    LOGGER->ERROR("failed to allocate address");
                    return;
                }

                addrReqData.startAddr = res.first;
                addrReqData.mask = res.second;
                int nonce = rand();
                addrReqData.nonce = nonce;

                AddrRequestDataWrapper wrapper;
                wrapper.data = addrReqData;
                protocol->rootAssignment[addrReqData.nonce] = wrapper;
                // start expire timer for assignment
                IOC->getTimer()->startTimer("assignment_expire_timer" + to_string(nonce), NDNADDR_ADDRREVOKEINTERVAL * 1000,
                                            [interfaceObj, nonce](string) -> bool {
                                                interfaceObj->getProtocol()->getCronjobHandler()->revokeAssignment(nonce);
                                                return false;
                                            });
            }

            auto encoded = addrReqData.encode();

            auto data = make_shared<NdnData>();
            data->setName(interest->getName());
            data->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});

            data->setContent(encoded.first, encoded.second.get());
            protocol->sendPacket(interfaceObj->getMacAddress(), data);
        } else if (splits[2] == "local") {
            if (interfaceObj->getLeader() != protocol->getRouterID()) {
                return;
            }
            // I am leader
            if (!interfaceObj->getAddrAssigned()) {
                return;
            }
            // I have got the addr
            AddrRequestData addrReqData;
            if (interfaceObj->assignment.find(router) != interfaceObj->assignment.end()) {
                addrReqData.startAddr = interfaceObj->assignment[router];
                LOGGER->INFOF(3, "assigning %s for request %s,nonce %d from storage", addrReqData.startAddr.toString().c_str(), interest->getName().c_str(),
                              addrReqData.nonce);

            } else {
                addrReqData.startAddr = interfaceObj->leaderAssignNextAddr();
                interfaceObj->assignment[router] = addrReqData.startAddr;
                LOGGER->INFOF(3, "assigning %s for request %s,nonce %d from pool", addrReqData.startAddr.toString().c_str(), interest->getName().c_str(),
                              addrReqData.nonce);
                LOGGER->VERBOSEF("%s %s ", addrReqData.startAddr.toString().c_str(), interfaceObj->assignment[router].toString().c_str());
            }
            addrReqData.nonce = rand();

            addrReqData.mask = interfaceObj->getIpv4Mask();
            auto encoded = addrReqData.encode();
            auto data = make_shared<NdnData>();
            data->setName(interest->getName());
            data->setPreferedInterfaces({{interfaceIndex, sourceMac}});
            data->setContent(encoded.first, encoded.second.get());
            protocol->sendPacket(interfaceObj->getMacAddress(), data);
        }
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void AddrRequestController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> data) {
    lock_guard<mutex> lockFunction(*(protocol->mutexLock));

    // string name="/addr/broadcast/req/"+to_string(protocol->getRouterID())+"/"+to_string(interfaceID)+"/"+to_string(requestSize);
    // tring name = "/addr/local/req/" + to_string(protocol->getRouterID()) + "/" + to_string(interfaceID);
    auto splits = split(data->getName(), "/");

    AddrRequestData addrData;
    auto encodePair = data->getContent();
    addrData.decode(encodePair.second.get(), encodePair.first);

    LOGGER->INFOF(3, "AddrRequestController::onReceiveData %s received, content %s", data->getName().c_str(), addrData.toString().c_str());
    if (splits[4] != to_string(protocol->routerID)) {
        return;
    }
    if (splits[2] == "broadcast") {
        int interfaceID = atoi(splits[5].c_str());
        auto interfaceObj = protocol->interfaces[interfaceID];

        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceID);
            return;
        }
        if (interfaceObj->getLeader() != protocol->getRouterID()) {
            return;
        }

        if (interfaceObj->getAddrAssigned()) {
            return;
        } else {
            if (!protocol->validator.checkAddress(addrData.startAddr, addrData.mask)) {
                LOGGER->ERRORF("invalid address detected, %s/%s", addrData.startAddr.toString().c_str(), addrData.mask.toString().c_str());
                return;
            }
            // protocol->mutexLock->unlock();
            // this_thread::sleep_for(std::chrono::milliseconds(3000));
            // protocol->mutexLock->lock();
            
            for (int i = 0; i < protocol->knownAddress.size(); i++) {
                if (addrData.startAddr.andMask(protocol->knownMask[i]) == protocol->knownAddress[i]) {
                    LOGGER->ERRORF("invalid address2 detected, %s/%s", addrData.startAddr.toString().c_str(), addrData.mask.toString().c_str());
                    return;
                }
            }
            interfaceObj->setAddrAssigned(true);
            interfaceObj->setAddrBlock(addrData.startAddr);
            interfaceObj->setIpv4Mask(addrData.mask);
            interfaceObj->setNextAddr(addrData.startAddr);
            interfaceObj->setIpv4Address(interfaceObj->leaderAssignNextAddr());
            interfaceObj->syncIpAddress();
            string name = "/addr/broadcast/conf/" + to_string(addrData.nonce) + "/" + addrData.startAddr.toString() + "/" + addrData.mask.toString();
            auto packet = make_shared<NdnInterest>();
            packet->setName(name);
            packet->setNonce(rand());
            LOGGER->INFOF(3, "sending %s", name.c_str());
            protocol->sendPacket(MacAddress("00:00:00:00:00:00"), packet);
        }
    } else if (splits[2] == "local") {
        // if (!protocol->validator.checkAddress(addrData.startAddr, addrData.mask)) {
        //     LOGGER->ERRORF("invalid address detected, %s/%s", addrData.startAddr.toString().c_str(), addrData.mask.toString().c_str());
        //     return;
        // }
        int interfaceID = atoi(splits[5].c_str());
        auto interfaceObj = protocol->interfaces[interfaceID];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceID);
            return;
        }
        interfaceObj->setAddrAssigned(true);
        interfaceObj->setIpv4Mask(addrData.mask);
        interfaceObj->setIpv4Address(addrData.startAddr);
        interfaceObj->syncIpAddress();

        IOC->getTimer()->cancelTimer(("addr_local_req_timer_" + to_string(interfaceID)));
    }
}