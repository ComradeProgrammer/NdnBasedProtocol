#ifndef __NDNROUTINGINTERFACESTATEDOWN_H_
#define __NDNROUTINGINTERFACESTATEDOWN_H_
#include "NdnRoutingInterfaceState.h"
class NdnRoutingInterfaceStateDown : public NdnRoutingInterfaceState {
    NdnRoutingInterfaceStateDown(NdnRoutingInterface* context)
        : NdnRoutingInterfaceState(context) {}
    virtual ~NdnRoutingInterfaceStateDown() override = default;
    virtual NdnRoutingInterfaceStateType getState() override { return DOWN; }

    virtual void processEvent(NdnRoutingInterfaceEventType event) override;
};
#endif