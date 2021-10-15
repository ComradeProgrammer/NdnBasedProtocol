#include "NdnRoutingNeighborStateInit.h"

#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
#include "ndnRouting/protocol/interface/neighbor/NdnRoutingNeighbor.h"
using namespace std;
void NdnRoutingNeighborStateInit::processEvent(NeighborEventType event) {
    switch (event) {
        case KILL_NEIGHBOR:
        case LL_DOWN:
            // TODO: implement handler
            break;
        case HELLO_RECEIVED: {
            // refresh the inactivity timer;
            auto timer = Timer::GetTimer();
            timer->cancelTimer("inactivity_timer_" + to_string(interface->getInterfaceID()) + "_" +
                               to_string(neighbor->getRouterID()));
            auto tmp = neighbor;
            timer->startTimer(
                "inactivity_timer_" + to_string(interface->getInterfaceID()) + "_" + to_string(neighbor->getRouterID()),
                NDNROUTING_ROUTERDEADINTERVAL * 1000, [tmp](string) -> bool {
                    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();
                    tmp->processEvent(INACTIVITY_TIMER);
                    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
                    return true;
                });
            break;
        }

        case TWOWAY_RECEIVED:
            // todo: implement 2-WAY handler
            break;
    }
}
