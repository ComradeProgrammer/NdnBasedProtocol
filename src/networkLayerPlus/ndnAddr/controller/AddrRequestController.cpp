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
        if (splits.size() != 7) {
            LOGGER->ERROR("invalid packet name");
            return;
        }
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
            LOGGER->VERBOSEF("here %s %s", res.first.toString().c_str(),res.second.toString().c_str());
            AddrRequestData addrReqData;
            addrReqData.startAddr = res.first;
            addrReqData.mask = res.second;
            LOGGER->VERBOSEF("here %s %s", addrReqData.startAddr.toString().c_str(),addrReqData.mask.toString().c_str());

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
    auto splits = split(data->getName(), "/");
    if (splits.size() != 7) {
        LOGGER->ERROR("invalid packet name");
        return;
    }
    AddrRequestData addrData;
    auto encodePair=data->getContent();
    addrData.decode(encodePair.second.get(),encodePair.first);

    LOGGER->INFOF(3,"AddrRequestController::onReceiveData %s received, content %s",data->getName().c_str(),addrData.toString().c_str());
}