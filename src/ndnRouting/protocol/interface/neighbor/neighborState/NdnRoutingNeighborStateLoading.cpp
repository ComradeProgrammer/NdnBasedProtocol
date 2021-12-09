#include "NdnRoutingNeighborStateLoading.h"

#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
#include "ndnRouting/protocol/interface/neighbor/NdnRoutingNeighbor.h"
using namespace std;
void NdnRoutingNeighborStateLoading::processEvent(NeighborEventType event) {
    auto timer = Timer::GetTimer();

    switch (event) {
        case KILL_NEIGHBOR:
        case LL_DOWN:
            // clear the inactivity timer
            neighbor->clear();
            timer->cancelTimer("inactivity_timer_" + to_string(interface->getInterfaceID()) + "_" +
                               to_string(neighbor->getRouterID()));
            neighbor->changeState(DOWN_STATE);
            break;
        case INACTIVITY_TIMER:
            neighbor->clear();
            neighbor->changeState(DOWN_STATE);
        case HELLO_RECEIVED: {
            // refresh the inactivity timer;
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
        case ONEWAY_RECEIVED: {
            neighbor->clear();
            neighbor->changeState(INIT_STATE);
            break;
        }
        case LOADING_DONE: {
            neighbor->cancelAllPendingLsaRequest();
            // when we turn to full state, we need to generate new lsa for myself, and send it out

            // first we search for the existing lsa
            auto protocol = NdnRoutingProtocol::getNdnRoutingProtocol();
            auto existingLsa = protocol->findLsa(ADJ, protocol->getRouterID());
            auto newLsa = protocol->generateLsa();
            // todo: add handler for overflow of seqnum
            if (existingLsa != nullptr) {
                newLsa->seqNum = existingLsa->seqNum + 1;
            }
            protocol->insertLsa(newLsa);
            protocol->rebuildRoutingTable();
            neighbor->sendInfoInterestDueToNeighbor(INFO_UP, newLsa->generateLSDigest());
            neighbor->changeState(FULL_STATE);
            break;
        }
    }
}
