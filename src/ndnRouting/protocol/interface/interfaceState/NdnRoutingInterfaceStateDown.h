#ifndef __NDNROUTINGINTERFACESTATEDOWN_H_
#define __NDNROUTINGINTERFACESTATEDOWN_H_
#include <mutex>
#include <string>
#include <thread>

#include "NdnRoutingInterfaceState.h"
#include "util/timer/Timer.h"
class NdnRoutingInterfaceStateDown : public NdnRoutingInterfaceState {
   public:
    NdnRoutingInterfaceStateDown(NdnRoutingInterface* context)
        : NdnRoutingInterfaceState(context) {}
    virtual ~NdnRoutingInterfaceStateDown() override = default;
    virtual NdnRoutingInterfaceStateType getState() override { return DOWN; }
    /**
     * @brief lock of protocol object should be have been acquired
     */
    virtual void processEvent(NdnRoutingInterfaceEventType event) override;

   private:
    /**
     * @brief lock of the protocol should have been acquired
     */
    void startSendingHelloMessage();
};
#endif