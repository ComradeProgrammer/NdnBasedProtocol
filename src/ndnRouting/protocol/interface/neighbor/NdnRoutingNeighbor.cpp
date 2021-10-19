#include"NdnRoutingNeighbor.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
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
    
}

void NdnRoutingNeighbor::createDatabaseSummary(){
    //choose the lsa which have a proper age 
    auto& adj=NdnRoutingProtocol::getNdnRoutingProtocol()->getAdjLsa();
    for(int i=0;i<adj.size();i++){
        if(adj[i].lsAge<NDN_ROUTING_MAX_AGE){
            databaseSummary.push_back(adj[i].generateLSDigest());
        }
    }
    auto& rch=NdnRoutingProtocol::getNdnRoutingProtocol()->getRchLsa();
    for(int i=0;i<rch.size();i++){
        if(rch[i].lsAge<NDN_ROUTING_MAX_AGE){
            databaseSummary.push_back(rch[i].generateLSDigest());
        }
    }

}
void NdnRoutingNeighbor::sendDDInterest(){
    string name="/routing/local/dd/"+to_string(routerID)+"/"+to_string(index);
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

    //unlock first because sendPacket will attain the lock of ndnprotocol
    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(macAddress, packet);
    //get the lock back because after return the lock needs to be attained
    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();

}