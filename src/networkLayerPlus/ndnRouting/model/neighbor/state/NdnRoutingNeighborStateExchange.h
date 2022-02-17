#ifndef __NDN_ROUTING_NEIGHBOR_STATE_EXCHANGE_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_EXCHANGE_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateExchange : public NdnRoutingNeighborState {
   public:
    NdnRoutingNeighborStateExchange(NdnRoutingNeighbor* _neighbor) : NdnRoutingNeighborState(_neighbor) {}
    virtual NeighborStateType getState() { return NeighborStateType::EXCHANGE; }
    virtual void processEvent(NeighborEventType event)override;
};
#endif