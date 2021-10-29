#ifndef __NDNROUTINGNEIGHBORSTATEFULL_H_
#define __NDNROUTINGNEIGHBORSTATEFULL_H_
#include "NdnRoutingNeighborState.h"

class NdnRoutingNeighborStateFull: public NdnRoutingNeighborState{
 public:
    NdnRoutingNeighborStateFull(NdnRoutingNeighbor* _neighbor,NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger = nullptr): NdnRoutingNeighborState(_neighbor,_interface,_logger){

    }
    virtual  NeighborStateType getState()override{return NeighborStateType::FULL_STATE;};
    virtual void processEvent(NeighborEventType event)override;
};
#endif