#ifndef __NDNROUTINGNEIGHBORSTATELOADING_H_
#define  __NDNROUTINGNEIGHBORSTATELOADING_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateLoading: public NdnRoutingNeighborState{
    public:
    NdnRoutingNeighborStateLoading(NdnRoutingNeighbor* _neighbor,NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger = nullptr): NdnRoutingNeighborState(_neighbor,_interface,_logger) {
       
    }
    virtual  NeighborStateType getState()override{return NeighborStateType::LOADING_STATE;};
    virtual void processEvent(NeighborEventType event)override;
};
#endif