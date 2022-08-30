#include "AddrChainController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"

using namespace std;
using nlohmann::json;
void AddrChainController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        auto splits = split(interest->getName(), "/");
        string lastHash = splits[4];
        int index = atoi(splits[5].c_str());
        int total = atoi(splits[6].c_str());
        protocol->chainBuffer[lastHash].push_back(interest);
        if (index < total - 1) {
            return;
        }
        if (protocol->chainBuffer[lastHash].size() != total) {
            LOGGER->ERROR("invalid chainbuffer");
            return;
        }

        char b[10000];
        char* ptr = b;
        int size = 0;
        for (int i = 0; i < total; i++) {
            auto p = protocol->chainBuffer[lastHash][i]->getApplicationParameters();
            memcpy(ptr, p.second.get(), p.first);
            size += p.first;
            ptr += p.first;
        }
        
        auto newChain = decodeBlockChain(b, size);
        LOGGER->INFOF(3, "AddrChainController::onReceiveInterest chain received, length %d , current length %d", newChain.chain.size(),
                      protocol->chain.chain.size());
        if (newChain.chain.size() > protocol->chain.chain.size()) {
            LOGGER->INFOF(3, "CHAINOPERATION REPLACEMENT: current chain %s", protocol->chainToString().c_str());
            protocol->chain = newChain;

            LOGGER->INFOF(3, "CHAINOPERATION REPLACEMENT:after insertion, current chain %s", protocol->chainToString().c_str());
            protocol->validator.clear();
            for (int i = 0; i < protocol->chain.chain.size(); i++) {
                if (protocol->chain.chain[i].getDataSize() != 0) {
                    string tmp=string(newChain.chain[i].getData());
                    json j;
                    j=j.parse(tmp);
                    if(j["type"]=="assignment"){
                        string assignment=j["assignmentInfo"];
                        protocol->validator.establishFromChainBlock(assignment);
                    }
                }
            }
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