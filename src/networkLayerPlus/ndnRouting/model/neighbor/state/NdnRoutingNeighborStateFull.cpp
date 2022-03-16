#include "NdnRoutingNeighborStateFull.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"
using namespace std;
void NdnRoutingNeighborStateFull::processEvent(NeighborEventType event) {
    switch (event) {
        case NeighborEventType::LL_DOWN:
        case NeighborEventType::KILL_NEIGHBOR:
        case NeighborEventType::INACTIVITY_TIMER: {
            neighbor->clear();
            neighbor->setState(NeighborStateType::DOWN);
            break;
        }
        case NeighborEventType::ONEWAY_RECEIVED: {
            neighbor->clear();
            neighbor->setState(NeighborStateType::INIT);
            break;
        }
        case NeighborEventType::HELLO_RECEIVED: {
            // restart the time by deleting it and add a new one
            string timerName = "inactivity_timer_" + to_string(neighbor->getBelongingInterface()->getInterfaceID()) + "_" + to_string(neighbor->getRouterID());
            neighbor->deleteTimer(timerName);

            NdnRoutingNeighbor* neighborForCapture = neighbor;
            IOC->getTimer()->startTimer(timerName, NDNROUTING_ROUTERDEADINTERVAL * 1000, [neighborForCapture](string) -> bool {
                neighborForCapture->getBelongingInterface()->getProtocol()->getCrobJobHandler()->neighborInactivityCronJob(neighborForCapture);
                return false;
            });
            neighbor->recordTimer(timerName);
            break;
        }
        case NeighborEventType::INVALID_HASH: {
            neighbor->createDatabaseSummary();
            neighbor->sendDDInterest();
            neighbor->setState(NeighborStateType::EXCHANGE);
            break;
        }
    }
}
