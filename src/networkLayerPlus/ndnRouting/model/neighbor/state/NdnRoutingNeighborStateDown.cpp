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
            NdnRoutingNeighbor* neighborForCapture = neighbor;
            timer->startTimer(timerName, NDNROUTING_ROUTERDEADINTERVAL * 1000, [neighborForCapture](string) -> bool {
                neighborForCapture->getBelongingInterface()->getProtocol()->getCrobJobHandler()->neighborInactivityCronJob(neighborForCapture);
                return false;
            });
            neighbor->recordTimer(timerName);
            neighbor->setState(NeighborStateType::INIT);
            break;
        }
    }
}