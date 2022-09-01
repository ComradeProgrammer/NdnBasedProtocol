#include "AddrCronjobController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
using namespace std;
void AddrCronjobController::sendingHelloMessageCronJob(int interfaceIndex) {
    try {
        protocol->lock();
        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERROR("AddrCronjobController::sendingHelloMessageCronJob: interface not found");
            return;
        }
        AddrHelloInterest helloInterest;
        helloInterest.routerID = protocol->routerID;
        helloInterest.leader = interfaceObj->getLeader();
        helloInterest.addr = interfaceObj->getIpv4Address();
        helloInterest.mask = interfaceObj->getIpv4Mask();
        helloInterest.interfaceNum = protocol->interfaces.size();
        helloInterest.helloInterval = NDNADDR_HELLOINTERVAL;

        auto packet = make_shared<NdnInterest>();
        auto encodePair = helloInterest.encode();
        packet->setName("/addr/local/hello");
        packet->setNonce(rand());
        packet->setApplicationParameters(encodePair.first, (const char*)encodePair.second.get());
        packet->setPreferedInterfaces({{interfaceIndex, MacAddress("ff:ff:ff:ff:ff:ff")}});

        protocol->sendPacket(interfaceObj->getMacAddress(), packet);
        protocol->unlock();
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void AddrCronjobController::waitingTimerCronJob(int interfaceIndex) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERROR("AddrCronjobController::sendingHelloMessageCronJob: interface not found");
            return;
        }
        RouterID newLeader = interfaceObj->calculateLeader();
        LOGGER->INFOF(3, "AddrCronjobController::waitingTimerCronJob: new leader is %d", newLeader);
        interfaceObj->setLeader(newLeader);

        if (interfaceObj->getStateType() == NdnAddrInterfaceStateType::WAITING) {
            if (newLeader == protocol->routerID) {
                // if I become the leader router
                interfaceObj->setState(NdnAddrInterfaceStateType::LEADER);
                if (protocol->getIsRoot()) {
                    // I am root,assign an address to myself
                    auto res = protocol->getAddressPool()->allocateAddress(interfaceObj->getNeighborNum());
                    interfaceObj->setAddrAssigned(true);
                    interfaceObj->setAddrBlock(res.first);
                    interfaceObj->setIpv4Mask(res.second);
                    interfaceObj->setNextAddr(res.first);
                    interfaceObj->setIpv4Address(interfaceObj->leaderAssignNextAddr());
                    interfaceObj->syncIpAddress();
                    //and write it to chain

                    string assignmentInfo=res.first.toString()+string(" ")+res.second.toString();
                    //LOGGER->INFOF(3,"CHAINOPERATION: current chain %s",protocol->chainToString().c_str());
                    //protocol->chain.generateNewBlock(assignmentInfo.c_str(), assignmentInfo.size()+1);
                    //LOGGER->INFOF(3,"CHAINOPERATION:after insertion, current chain %s",protocol->chainToString().c_str());
                    protocol->blockBuffer.push_back(assignmentInfo);

                    //and broadcast it
                    string name = "/addr/broadcast/conf/" + to_string(rand()) + "/" + res.first.toString() + "/" + res.second.toString();
                    auto packet = make_shared<NdnInterest>();
                    packet->setName(name);
                    packet->setNonce(rand());
                    LOGGER->INFOF(3, "sending %s", name.c_str());
                    protocol->sendPacket(MacAddress("00:00:00:00:00:00"), packet);


                } else {
                    // I am not root
                    interfaceObj->sendBroadcastAddrRequest();
                }
            } else {
                // if I become the normal router
                interfaceObj->setState(NdnAddrInterfaceStateType::NORMAL);
                // periodically request for ip address
                IOC->getTimer()->startTimer("addr_local_req_timer_" + to_string(interfaceIndex), NDNADDR_ADDRREQINTERVAL * 1000,
                                            [interfaceObj](string) -> bool {
                                                interfaceObj->getProtocol()->getCronjobHandler()->localAddrRequestCronJob(interfaceObj->getInterfaceID());
                                                return true;
                                            });
            }
        }
        //
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void AddrCronjobController::localAddrRequestCronJob(int interfaceIndex) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERROR("AddrCronjobController::sendingHelloMessageCronJob: interface not found");
            return;
        }
        interfaceObj->sendLocalAddrRequest();
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void AddrCronjobController::inactiveTimer(NdnAddrNeighbor* neighbor) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        neighbor->processEvent(NdnAddrNeighborEventType::KILL_NEIGHBOR);
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void AddrCronjobController::revokeAssignment(int nonce) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        LOGGER->WARNINGF("AddrCronjobController::revokeAssignment, nonce %d",nonce);
        if(protocol->rootAssignment.find(nonce)==protocol->rootAssignment.end()){
            LOGGER->ERRORF("failed to find record of nonce %d", nonce);
            return;
        }
        auto data=protocol->rootAssignment[nonce].data;
        protocol->addressPool->returnAddress(data.startAddr, data.mask);
        protocol->rootAssignment.erase(nonce);
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}