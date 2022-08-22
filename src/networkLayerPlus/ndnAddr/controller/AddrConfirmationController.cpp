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
        LOGGER->INFO(3,"here1");
        if(protocol->rootAssignment.find(nonce)==protocol->rootAssignment.end()){
            return;
        }

        IOC->getTimer()->cancelTimer("assignment_expire_timer" + to_string(nonce));

        AddrRequestDataWrapper wrapper=protocol->rootAssignment[nonce];
        string assignmentInfo=wrapper.data.startAddr.toString()+string(" ")+wrapper.data.mask.toString();

        //LOGGER->INFOF(3,"CHAINOPERATION: current chain %s",protocol->chainToString().c_str());
        protocol->blockBuffer.push_back(assignmentInfo);
        //protocol->chain.generateNewBlock(assignmentInfo.c_str(), assignmentInfo.size()+1);
        //LOGGER->INFOF(3,"CHAINOPERATION:after insertion, current chain %s",protocol->chainToString().c_str());
        
    } catch (exception e) {

        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void AddrConfirmationController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}