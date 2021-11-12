#ifndef __NDNROUTINGNEIGHBORSTATEDOWN_H_
#define __NDNROUTINGNEIGHBORSTATEDOWN_H_
#include "NdnRoutingNeighborState.h"
#include "util/timer/Timer.h"
class NdnRoutingNeighborStateDown : public NdnRoutingNeighborState {
   public:
    NdnRoutingNeighborStateDown(NdnRoutingNeighbor* _neighbor, NdnRoutingInterface* _interface,
                                std::shared_ptr<Logger> _logger = nullptr)
        : NdnRoutingNeighborState(_neighbor, _interface, _logger) {}
    virtual NeighborStateType getState() override { return NeighborStateType::DOWN_STATE; };
    virtual void processEvent(NeighborEventType event) override;
};

#endif