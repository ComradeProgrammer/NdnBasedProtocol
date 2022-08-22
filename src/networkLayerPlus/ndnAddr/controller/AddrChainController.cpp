#include "AddrChainController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"

using namespace std;
void AddrChainController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));
        if (!protocol->getIsRoot()) {
            return;
        }
        auto data = interest->getApplicationParameters();
        auto newChain = decodeBlockChain(data.second.get(), data.first);
        LOGGER->INFOF(3,"AddrChainController::onReceiveInterest chain received, length %d , current length %d",newChain.chain.size(), protocol->chain.chain.size());
        if (newChain.chain.size() > protocol->chain.chain.size()) {
            LOGGER->INFOF(3, "CHAINOPERATION REPLACEMENT: current chain %s", protocol->chainToString().c_str());
            protocol->chain = newChain;
            LOGGER->INFOF(3, "CHAINOPERATION REPLACEMENT:after insertion, current chain %s",  protocol->chainToString().c_str());
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void AddrChainController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}