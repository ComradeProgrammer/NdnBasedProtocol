#ifndef __NDN_ROUTING_NEIGHBOR_STATE_DOWN_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_DOWN_H_
#include"NdnRoutingNeighborState.h"
#include"ioc.h"
class NdnRoutingNeighborStateDown:public NdnRoutingNeighborState{
   public:
    NdnRoutingNeighborStateDown(NdnRoutingNeighbor* _neighbor):NdnRoutingNeighborState(_neighbor){}
    virtual NeighborStateType getState(){return NeighborStateType::DOWN;}
    virtual void processEvent(NeighborEventType event);

};
#endif