#include "HelloController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
using namespace std;
void HelloController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        // resolve the content of hello packet
        auto helloInfoData = interest->getApplicationParameters();
        HelloInterestPack helloInfo;
        helloInfo.decode(helloInfoData.second.get(), helloInfoData.first);

        LOGGER->INFOF("HelloController::onReceiveInterest at interface %d, source %s, content %s", interfaceIndex, sourceMac.toString().c_str(),
                      helloInfo.toString().c_str());

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        // check the mask, interval and routerDeadInterval
        if (!(helloInfo.networkMask == interfaceObj->getIpv4Mask())) {
            LOGGER->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible mask");
            return;
        }
        if (helloInfo.routerDeadInterval != NDNROUTING_ROUTERDEADINTERVAL) {
            LOGGER->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible router dead interval");
            return;
        }

        // if related neighbor is not recorded, create a new one
        auto neighborObj = interfaceObj->getNeighborByRouterID(helloInfo.routerId);
        if (neighborObj == nullptr) {
            neighborObj = make_shared<NdnRoutingNeighbor>(interfaceObj.get());
            neighborObj->setRouterID(helloInfo.routerId);
            neighborObj->setMacAddress(sourceMac);
            neighborObj->setIpv4Address(helloInfo.interfaceIP);
            neighborObj->setIpv4Mask(helloInfo.networkMask);
            interfaceObj->addNeighbor(neighborObj);
        }
        neighborObj->processEvent(NeighborEventType::HELLO_RECEIVED);
        // now resolve the neighbor info incorporated in the hello packet
        // if we find that current instance shows up in hellopacket, then just go directly into 2-way
        bool found = false;
        for (auto i : helloInfo.neighbor) {
            if (i == interfaceObj->getIpv4Address()) {
                // found
                neighborObj->processEvent(NeighborEventType::TWOWAY_RECEIVED);
                found = true;
                break;
            }
        }
        if (!found) {
            neighborObj->processEvent(NeighborEventType::ONEWAY_RECEIVED);
        }
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void HelloController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {
    // should not reach here. Hello has no data.
}
