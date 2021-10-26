#include"NdnRoutingNeighbor.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"
using namespace std;
NdnRoutingNeighbor::NdnRoutingNeighbor(NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger ):interface(_interface){
    logger=Logger::getDefaultLoggerIfNull(_logger);
    state=make_shared<NdnRoutingNeighborStateDown>(this,interface,logger);
}

void NdnRoutingNeighbor::changeState(NeighborStateType stateType){
    logger->INFOF("interface %d neighbor %s(rid %d) change to state %s from %s",
        interface->getInterfaceID(),
        ipAddress.toString().c_str(),routerID,
        getNameForNeighborState(state->getState()).c_str(),
        getNameForNeighborState(stateType).c_str()
    );
    shared_ptr<NdnRoutingNeighborState>newState=nullptr;
    switch(stateType){
        case DOWN_STATE:
            newState=make_shared<NdnRoutingNeighborStateDown>(this,interface,logger);
        break;
        case INIT_STATE:
            newState=make_shared<NdnRoutingNeighborStateInit>(this,interface,logger);
        break;
        case EXCHANGE_STATE:
            newState=make_shared<NdnRoutingNeighborStateExchange>(this,interface,logger);
        break;
        case LOADING_STATE:
        break;
        case FULL_STATE:
        break;
    }
    state=newState;

}


void  NdnRoutingNeighbor::processEvent(NeighborEventType e){
    logger->INFOF(
        "interface %d, neighbor %s(rid:%d) process Event %s, current state %s",
        interface->getInterfaceID(),
        ipAddress.toString().c_str(),routerID,
        getNameForNeighborEvent(e).c_str(),
        getNameForNeighborState(state->getState()).c_str()
    );
    state->processEvent(e);
}

void NdnRoutingNeighbor::clear(){
    //1. remove the hello timer
}

void NdnRoutingNeighbor::createDatabaseSummary(){
    //choose the lsa which have a proper age 
    auto& adj=NdnRoutingProtocol::getNdnRoutingProtocol()->getAdjLsa();
    for(int i=0;i<adj.size();i++){
        if(adj[i]->lsAge<NDN_ROUTING_MAX_AGE){
            databaseSummary.push_back(adj[i]->generateLSDigest());
        }
    }
    auto& rch=NdnRoutingProtocol::getNdnRoutingProtocol()->getRchLsa();
    for(int i=0;i<rch.size();i++){
        if(rch[i]->lsAge<NDN_ROUTING_MAX_AGE){
            databaseSummary.push_back(rch[i]->generateLSDigest());
        }
    }

}
void NdnRoutingNeighbor::sendDDInterest(){
    string name="/routing/local/dd/"+to_string(routerID)+"/"+to_string(recvingIndex);
    //construct a dd interest pack
    DDInterestPack interestPack;
    interestPack.neighbor=routerID;
    auto encodePair=interestPack.encode();
    //construct a dd interest
    auto packet = make_shared<NdnInterest>(logger);
    packet->setName(name);
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());
    packet->setPreferedInterfaces(
        {{interface->getInterfaceID(), macAddress}});
    //todo: add retransmission mechanism
    //unlock first because sendPacket will attain the lock of ndnprotocol
    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(interface->getMacAddress(), packet);
    //get the lock back because after return the lock needs to be attained
    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();

}

void NdnRoutingNeighbor::sendDDData(int requestIndex,string name){
   
    auto packet = make_shared<NdnData>(logger);
    packet->setName(name);
    if(requestIndex==0){
        //we need to construct the transmitting list first
        //take fragment mechanism into consideration
        // DATA-TYPE(9) TLV-LENGTH(9) Name(9+9+namesize) Content(9+9+contentsize)
        //content size=8+12*num of digest
        int reservedLength=9+9+9+9+9+9+8+name.size()+1;
        int remainingLength=MTU-reservedLength;
        int numberofDigest=remainingLength/12;
        for(int i=0;i<databaseSummary.size();i++){
            if(i%numberofDigest==0){
                DDDataPack dataPack;
                dataPack.neighbor=interface->getInterfaceID();
                dataPack.interfaceMTU=MTU;
                dataPack.numberOfDDPackets=databaseSummary.size()/numberofDigest+1;
                ddList.push_back(dataPack);
            }
            ddList[i/numberofDigest].ls.push_back(databaseSummary[i]);
        }
        if(databaseSummary.size()==0){
            DDDataPack dataPack;
            dataPack.neighbor=interface->getInterfaceID();
            dataPack.interfaceMTU=MTU;
            dataPack.numberOfDDPackets=0;
            ddList.push_back(dataPack);
        }
        //send out the dddata of given index
        auto encodedPair=ddList[requestIndex].encode();
        packet->setContent(encodedPair.first,encodedPair.second.get());
        packet->setPreferedInterfaces(
        {{interface->getInterfaceID(), macAddress}});
        NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
        NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(interface->getMacAddress(), packet);
        //get the lock back because after return the lock needs to be attained
        NdnRoutingProtocol::getNdnRoutingProtocol()->lock();
        
    }
}
void NdnRoutingNeighbor::onReceiveDDInterset(std::shared_ptr<NdnInterest> interest){

    //if we are in init state, just turn to the exchange state
    if(state->getState()==INIT_STATE){
        processEvent(TWOWAY_RECEIVED);
    }
        logger->ERRORF("NdnRoutingNeighbor::onReceiveDDInterset  name %s, sending index %d",interest->getName().c_str(),sendingIndex);

    //we allow asking for retransmission, but we don't allow the request of lsa earlier than that
    auto splits=split(interest->getName(),"/");
    if(splits.size()<6){
        logger->ERRORF("NdnRoutingNeighbor::onReceiveDDInterset invalid dd interest name %s",interest->getName().c_str());
        return;
    }
    int requestedIndex=atoi(splits[5].c_str());
    if((requestedIndex==sendingIndex || requestedIndex==sendingIndex -1)&&(requestedIndex<databaseSummary.size()||requestedIndex==0)){
        sendDDData(requestedIndex,interest->getName());
        sendingIndex=requestedIndex+1;
    }else{
        //the requested index is not allowed to be smaller than recorded index-1 or even greater
        //TODO: implement the fault handler
        logger->ERRORF("NdnRoutingNeighbor::onReceiveDDInterset invalid index in name %s, currenting sending index %d",interest->getName().c_str(),sendingIndex);
        return;
    }
}
void NdnRoutingNeighbor::onReceiveDDData(shared_ptr<NdnData> data){
    if(state->getState()!=EXCHANGE_STATE){
        logger->INFOF("NdnRoutingNeighbor::onReceiveDDData: dd packet dropped due to incorrect state. packet %s, current state %s",data->toString().c_str(),getNameForNeighborState(state->getState()).c_str());
    }
    //check the index, drop it if we have received this
    auto splits=split(data->getName(),"/");
    if(splits.size()<6){
        logger->ERRORF("NdnRoutingNeighbor::onReceiveDDData invalid dd data name %s",data->getName().c_str());
        return;
    }
    int offeredIndex=atoi(splits[5].c_str());
    if(offeredIndex<recvingIndex){
        logger->ERRORF("NdnRoutingNeighbor::onReceiveDDData dropped due to expired index %s",data->getName().c_str());
        return;
    }else if(offeredIndex>recvingIndex){
        //TODO: implement the fault handler
    }else{
        //remove the dd interest retransmission  timer

        DDDataPack dataPack;
        auto contentPair=data->getContent();
        dataPack.decode(contentPair.second.get(),contentPair.first);
        //check every digest listed in data
        for(int i=0;i<dataPack.ls.size();i++){
            if(dataPack.ls[i].linkStateType!=ADJ &&dataPack.ls[i].linkStateType!=RCH){
                logger->ERROR("NdnRoutingNeighbor::onReceiveDDData dropped due to invalid linkstate digest type");
                return;
            }
            auto protocol=NdnRoutingProtocol::getNdnRoutingProtocol();
            auto existingLsa=protocol->findLsa(dataPack.ls[i].linkStateType,dataPack.ls[i].routerID);
            if(existingLsa==nullptr){
                //if we haven't got the lsa yet, request for it
                localLsaPendingRequestList.push_back(dataPack.ls[i]);
                sendLocalLsaInterestWithRetransmission(dataPack.ls[i]);
            }else{
                if(existingLsa->generateLSDigest()<dataPack.ls[i] && !protocol->inBroadcastLsaPendingRequestList(dataPack.ls[i].linkStateType,dataPack.ls[i].routerID,dataPack.ls[i].sequenceNum)){
                    //if the existing lsa has expired, request for it
                    localLsaPendingRequestList.push_back(dataPack.ls[i]);
                    sendLocalLsaInterestWithRetransmission(dataPack.ls[i]);
                }
            }
        }
        recvingIndex++;
        logger->INFOF("here %d",dataPack.numberOfDDPackets);
        if(recvingIndex==dataPack.numberOfDDPackets||dataPack.numberOfDDPackets==0){
            //todo: change to loading state
        }else{
            sendDDInterest();
        }
        
    }
}

void NdnRoutingNeighbor::sendLocalLsaInterestWithRetransmission(LinkStateDigest digest){
    sendLocalLsaInterest(digest);
    Timer::GetTimer()->startTimer(
        string("lsa_interest_timer")+to_string(interface->getInterfaceID())+"_"+to_string(routerID)+"_"+to_string(digest.linkStateType)+"_"+to_string(digest.sequenceNum),
        DD_RETRANSMISSION_TIMER*1000,
        [digest,this](string)->bool{
            NdnRoutingProtocol::getNdnRoutingProtocol()->lock();
            for(auto tmp:localLsaPendingRequestList){
                if(tmp.linkStateType==digest.linkStateType && tmp.routerID==digest.routerID && tmp.sequenceNum==digest.sequenceNum){
                    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
                    return false;
                }
            }
            sendLocalLsaInterest(digest);
            //now only retransmit for 1 time
            NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
            return false;
        }
    );
}

void NdnRoutingNeighbor::sendLocalLsaInterest(LinkStateDigest digest){
    string name="/routing/local/LSA/"+getNameForLinkStateType(digest.linkStateType)+"/"+to_string(digest.routerID)+"/"+to_string(digest.sequenceNum);
    LsaInterestPack lsaInterestPack;
    lsaInterestPack.routerID=digest.routerID;
    lsaInterestPack.sequenceNum=digest.sequenceNum;
    lsaInterestPack.lsType=digest.linkStateType;

    auto encodePair=lsaInterestPack.encode();

    auto packet = make_shared<NdnInterest>(logger);
    packet->setName(name);
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first,encodePair.second.get());
    packet->setPreferedInterfaces(
        {{interface->getInterfaceID(), macAddress}});

    //unlock first because sendPacket will attain the lock of ndnprotocol
    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(interface->getMacAddress(), packet);
    //get the lock back because after return the lock needs to be attained
    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();

}