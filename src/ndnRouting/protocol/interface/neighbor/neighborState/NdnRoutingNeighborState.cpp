#include "NdnRoutingNeighborState.h"
using namespace std;
string getNameForNeighborEvent(NeighborEventType eventType) {
    switch (eventType) {
        case HELLO_RECEIVED:
            return "HELLO_RECEIVED";
            break;
        case TWOWAY_RECEIVED:
            return "TWOWAY_RECEIVED";
            break;
        case ONEWAY_RECEIVED:
            return "ONEWAY_RECEIVED";
            break;
        case KILL_NEIGHBOR:
            return "KILL_NEIGHBOR";
            break;
        case INACTIVITY_TIMER:
            return "INACTIVITY_TIMER";
            break;
        case LL_DOWN:
            return "LL_DOWN";
            break;
        case EXCHANGE_DONE:
            return "EXCHANGE_DONE";
            break;
        case LOADING_DONE:
            return "LOADING_DONE";
            break;
        case DD_RETRANSMISSION_TIMER:
            return "DD_RETRANSMISSION_TIMER";
            break;
        case REQ_RETRANSMISSION_TIMER:
            return "REQ_RETRANSMISSION_TIMER";
            break;
    }
    return "";
}
string getNameForNeighborState(NeighborStateType stateType) {
    switch (stateType) {
        case DOWN_STATE:
            return "DOWN_STATE";
            break;
        case INIT_STATE:
            return "INIT_STATE";
            break;
        case EXCHANGE_STATE:
            return "EXCHANGE_STATE";
            break;
        case LOADING_STATE:
            return "LOADING_STATE";
            break;
        case FULL_STATE:
            return "FULL_STATE";
            break;
    }
    return "";
}