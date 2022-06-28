#include "AddrHelloController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"

using namespace std;
void AddrHelloController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        auto data = interest->getApplicationParameters();
        AddrHelloInterest helloInfo;
        helloInfo.decode(data.second.get(), data.first);
        LOGGER->INFOF(3, "AddrHelloController::onReceiveInterest at interface %d, source %s, content %s", interfaceIndex, sourceMac.toString().c_str(),
                      helloInfo.toString().c_str());

        if (helloInfo.helloInterval != NDNADDR_HELLOINTERVAL) {
            LOGGER->WARNING("AddrHelloController::onReceiveHelloInterest packet is dropped due to incompatible hellointerval");
            return;
        }
        auto interfaceObj = protocol->interfaces[interfaceIndex];
        auto neighborObj = interfaceObj->getNeighborByRouterID(helloInfo.routerID);
        if (neighborObj == nullptr) {
            neighborObj = make_shared<NdnAddrNeighbor>(interfaceObj.get());
            neighborObj->setIsStateUp(false);
            neighborObj->setMacAddress(sourceMac);
            neighborObj->setRouterID(helloInfo.routerID);
            interfaceObj->addNeighbor(neighborObj);
        }
        neighborObj->setNeighborLeader(helloInfo.leader);
        neighborObj->setInterfaceNum(helloInfo.interfaceNum);
        neighborObj->processEvent(NdnAddrNeighborEventType::HELLO_RECEIVE);

        if(interfaceObj->getStateType()==NdnAddrInterfaceStateType::DOWN){
            interfaceObj->processInterfaceEvent(NdnAddrInterfaceEventType::INTERFACE_UP);
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}

void AddrHelloController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}