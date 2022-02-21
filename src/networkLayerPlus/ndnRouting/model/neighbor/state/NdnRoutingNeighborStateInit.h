#ifndef __NDN_ROUTING_NEIGHBOR_STATE_INIT_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_INIT_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateInit : public NdnRoutingNeighborState {
   public:
    NdnRoutingNeighborStateInit(NdnRoutingNeighbor* _neighbor) : NdnRoutingNeighborState(_neighbor) {}
    virtual NeighborStateType getState() { return NeighborStateType::INIT; }
    virtual void processEvent(NeighborEventType event);
};
#endif