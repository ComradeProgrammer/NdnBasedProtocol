#ifndef __NDN_ROUTING_NEIGHBOR_STATE_LOADING_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_LOADING_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateLoading:public NdnRoutingNeighborState{
    public:
    NdnRoutingNeighborStateLoading(NdnRoutingNeighbor* _neighbor) : NdnRoutingNeighborState(_neighbor) {}
    virtual NeighborStateType getState() { return NeighborStateType::LOADING; }
    virtual void processEvent(NeighborEventType event);
};
#endif