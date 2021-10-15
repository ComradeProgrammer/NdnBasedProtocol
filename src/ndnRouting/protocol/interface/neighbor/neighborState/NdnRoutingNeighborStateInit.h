#ifndef __NDNROUTINGNEIGHBORSTATEINIT_H_
#define __NDNROUTINGNEIGHBORSTATEINIT_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateInit:public NdnRoutingNeighborState{
    public:
    NdnRoutingNeighborStateInit(NdnRoutingNeighbor* _neighbor,NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger = nullptr): NdnRoutingNeighborState(_neighbor,_interface,_logger){

    }
    virtual NeighborStateType getState(){return NeighborStateType::INIT_STATE;}

    virtual void processEvent(NeighborEventType event)override;

};

#endif