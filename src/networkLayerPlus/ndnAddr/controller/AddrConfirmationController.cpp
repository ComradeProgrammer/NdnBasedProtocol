#include "AddrConfirmationController.h"

#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
using namespace std;
void AddrConfirmationController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        // string name = "/addr/broadcast/conf/" + to_string(addrData.nonce);
        
        auto splits = split(interest->getName(), "/");
        Ipv4Address a(splits[5]);
        Ipv4Address m(splits[6]);
        protocol->knownAddress.push_back(a);
        protocol->knownMask.push_back(m);
        LOGGER->VERBOSEF("record used address, %s %s", a.toString().c_str(), m.toString().c_str());

        //remove invalid addr
        for(auto p: protocol->interfaces){
            Ipv4Address addr=p.second->getIpv4Address();
            if(addr.andMask(m)==a){
                LOGGER->VERBOSEF("invalid address detected, %s %s", a.toString().c_str(), m.toString().c_str());
                p.second->setIpv4Address(Ipv4Address("10.0.0.1"));
                p.second->syncIpAddress();
            }
        }

        if (!protocol->getIsRoot()) {
            return;
        }
        int nonce = atoi(splits[4].c_str());
       
        LOGGER->VERBOSEF("record used address, %s %s", a.toString().c_str(), m.toString().c_str());
        if (protocol->rootAssignment.find(nonce) == protocol->rootAssignment.end()) {
            return;
        }

        IOC->getTimer()->cancelTimer("assignment_expire_timer" + to_string(nonce));

        AddrRequestDataWrapper wrapper = protocol->rootAssignment[nonce];
        string assignmentInfo = wrapper.data.startAddr.toString() + string(" ") + wrapper.data.mask.toString();

        // LOGGER->INFOF(3,"CHAINOPERATION: current chain %s",protocol->chainToString().c_str());
        protocol->blockBuffer.push_back(assignmentInfo);
        // protocol->chain.generateNewBlock(assignmentInfo.c_str(), assignmentInfo.size()+1);
        // LOGGER->INFOF(3,"CHAINOPERATION:after insertion, current chain %s",protocol->chainToString().c_str());

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void AddrConfirmationController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}