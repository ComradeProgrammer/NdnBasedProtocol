#include "RoutingChainController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"

using namespace std;
void RoutingChainController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        auto splits = split(interest->getName(), "/");
        string lastHash = splits[4];
        int index = atoi(splits[5].c_str());
        int total = atoi(splits[6].c_str());

        if (index < total - 1) {
            return;
        }

        char b[10000];
        char* ptr = b;
        int size = 0;

        auto newChain = decodeBlockChain(b, size);
        LOGGER->INFOF(3, "AddrChainController::onReceiveInterest chain received, length %d , current length %d", newChain.chain.size(),
                      protocol->chain.chain.size());
        if (newChain.chain.size() > protocol->chain.chain.size()) {
            LOGGER->INFOF(3, "CHAINOPERATION REPLACEMENT: current chain %s", protocol->chainToString().c_str());
            protocol->chain = newChain;
            protocol->blockBuffer.clear();
            LOGGER->INFOF(3, "CHAINOPERATION REPLACEMENT:after insertion, current chain %s", protocol->chainToString().c_str());
        }

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void RoutingChainController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}