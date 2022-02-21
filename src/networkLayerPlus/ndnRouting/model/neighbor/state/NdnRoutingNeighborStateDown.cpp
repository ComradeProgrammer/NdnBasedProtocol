#include "NdnRoutingNeighborStateDown.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"

using namespace std;
void NdnRoutingNeighborStateDown::processEvent(NeighborEventType event) {
    switch (event) {
        case NeighborEventType::HELLO_RECEIVED: {
            // start inactivity timer
            auto timer = IOC->getTimer();
            string timerName = "inactivity_timer_" + to_string(neighbor->getBelongingInterface()->getInterfaceID()) + "_" + to_string(neighbor->getRouterID());
            timer->startTimer(timerName, NDNROUTING_ROUTERDEADINTERVAL * 1000, [this](string) -> bool {
                neighbor->getBelongingInterface()->getProtocol()->getCrobJobHandler()->neighborInactivityCronJob(neighbor);
                return false;
            });
            neighbor->recordTimer(timerName);
            neighbor->setState(NeighborStateType::INIT);
            break;
        }
    }
}