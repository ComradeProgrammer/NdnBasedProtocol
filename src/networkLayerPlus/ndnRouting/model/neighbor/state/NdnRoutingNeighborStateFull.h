#ifndef __NDN_ROUTING_NEIGHBOR_STATE_FULL_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_FULL_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateFull : public NdnRoutingNeighborState {
   public:
    NdnRoutingNeighborStateFull(NdnRoutingNeighbor* _neighbor) : NdnRoutingNeighborState(_neighbor) {}
    virtual NeighborStateType getState() { return NeighborStateType::FULL; }
    virtual void processEvent(NeighborEventType event);
};
#endif