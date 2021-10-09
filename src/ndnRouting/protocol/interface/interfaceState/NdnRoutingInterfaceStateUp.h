#ifndef __NDNROUTINGINTERFACESTATEUP_H_
#define __NDNROUTINGINTERFACESTATEUP_H_
#include "NdnRoutingInterfaceState.h"
class NdnRoutingInterfaceStateUp : public NdnRoutingInterfaceState {
   public:
    NdnRoutingInterfaceStateUp(NdnRoutingInterface* context)
        : NdnRoutingInterfaceState(context) {}
    virtual ~NdnRoutingInterfaceStateUp() = default;
    virtual NdnRoutingInterfaceStateType getState() override { return UP; }

    /**
     * @brief lock of protocol object will be acquired
     */
    virtual void processEvent(NdnRoutingInterfaceEventType event) override;
};

#endif