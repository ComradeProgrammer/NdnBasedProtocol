#include "AddrRequestController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"

using namespace std;
void AddrRequestController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        LOGGER->INFOF(3, "AddrRequestController::onReceiveInterest AddrRequest received %s", interest->getName().c_str());
        // string name="/addr/broadcast/req/"+to_string(protocol->getRouterID())+"/"+to_string(interfaceID)+"/"+to_string(requestSize);

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceIndex);
            return;
        }
        auto splits = split(interest->getName(), "/");
        RouterID router = atoRID(splits[4]);
        if (splits[2] == "broadcast") {
            if (!protocol->getIsRoot()) {
                // is not root, ignore and let the packet  be transmitted
                return;
            }
            int num = atoi(splits[6].c_str());
            auto res = protocol->getAddressPool()->allocateAddress(num);

            if (res.first == Ipv4Address("0.0.0.0")) {
                LOGGER->ERROR("failed to allocate address");
                return;
            }
            AddrRequestData addrReqData;
            addrReqData.startAddr = res.first;
            addrReqData.mask = res.second;

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
                addrReqData.startAddr == interfaceObj->assignment[router];
            } else {
                addrReqData.startAddr = interfaceObj->leaderAssignNextAddr();
                interfaceObj->assignment[router] = addrReqData.startAddr;
            }

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
    // string name="/addr/broadcast/req/"+to_string(protocol->getRouterID())+"/"+to_string(interfaceID)+"/"+to_string(requestSize);
    auto splits = split(data->getName(), "/");

    AddrRequestData addrData;
    auto encodePair = data->getContent();
    addrData.decode(encodePair.second.get(), encodePair.first);

    LOGGER->INFOF(3, "AddrRequestController::onReceiveData %s received, content %s", data->getName().c_str(), addrData.toString().c_str());
    if (splits[2] == "broadcast") {
        int interfaceID = atoi(splits[5].c_str());
        auto interfaceObj = protocol->interfaces[interfaceID];
        if (interfaceObj == nullptr) {
            LOGGER->ERRORF("interface %d not found", interfaceID);
            return;
        }

        if (interfaceObj->getAddrAssigned()) {
            return;
        } else {
            interfaceObj->setAddrAssigned(true);
            interfaceObj->setAddrBlock(addrData.startAddr);
            interfaceObj->setIpv4Mask(addrData.mask);
            interfaceObj->setNextAddr(addrData.startAddr);
            interfaceObj->setIpv4Address(interfaceObj->leaderAssignNextAddr());
            interfaceObj->syncIpAddress();
            // todo: send addr assign interest
        }
    } else if (splits[2] == "local") {
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