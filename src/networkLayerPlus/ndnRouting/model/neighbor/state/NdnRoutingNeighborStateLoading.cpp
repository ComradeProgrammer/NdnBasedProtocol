#include "NdnRoutingNeighborStateLoading.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "networkLayerPlus/ndnRouting/model/interface/NdnRoutingInterface.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"
using namespace std;
void NdnRoutingNeighborStateLoading::processEvent(NeighborEventType event) {
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
        case NeighborEventType::LOADING_DONE: {
            // when we turn to full state, we need to generate new lsa for myself, and send it out
            // first we search for the existing lsa
            auto lsa = neighbor->getBelongingInterface()->getProtocol()->generateLsa();
            // todo:implement
            // protocol->rebuildRoutingTable();

            //todo: replace this part into a convenient way(add a new state)
            neighbor->getBelongingInterface()->getProtocol()->registerParents();
            neighbor->getBelongingInterface()->getProtocol()->sendInfoToChildren(lsa);
            neighbor->setState(NeighborStateType::FULL);
        }
    }
}
