#include"NdnRoutingNeighbor.h"
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
