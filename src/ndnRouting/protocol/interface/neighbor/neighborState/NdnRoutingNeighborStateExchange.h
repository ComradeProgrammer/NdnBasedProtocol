#ifndef __NDNROUTINGNEIGHBORSTATEEXCHANGE_H_
#define __NDNROUTINGNEIGHBORSTATEEXCHANGE_H_
#include "NdnRoutingNeighborState.h"
class NdnRoutingNeighborStateExchange : public NdnRoutingNeighborState {
   public:
    NdnRoutingNeighborStateExchange(NdnRoutingNeighbor* _neighbor, NdnRoutingInterface* _interface,
                                    std::shared_ptr<Logger> _logger = nullptr)
        : NdnRoutingNeighborState(_neighbor, _interface, _logger) {}
    virtual NeighborStateType getState() override { return NeighborStateType::EXCHANGE_STATE; };
    virtual void processEvent(NeighborEventType event) override;
};

#endif