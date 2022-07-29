#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
#include "AddrConfirmationController.h"
using namespace std;
void AddrConfirmationController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        // string name = "/addr/broadcast/conf/" + to_string(addrData.nonce);
        if(!protocol->getIsRoot()){
            return;
        }
        auto splits = split(interest->getName(), "/");
        int nonce = atoi(splits[4].c_str());
        IOC->getTimer()->cancelTimer("assignment_expire_timer" + to_string(nonce));
        
    } catch (exception e) {

        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void AddrConfirmationController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}