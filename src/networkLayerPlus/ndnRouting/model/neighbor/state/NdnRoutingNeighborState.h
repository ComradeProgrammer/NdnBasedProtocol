#ifndef __NDN_ROUTING_NEIGHBOR_STATE_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_H_
#include <memory>
#include <string>
enum class NeighborStateType { DOWN = 0, INIT = 1, EXCHANGE = 2, LOADING = 4, FULL = 8 };
enum class NeighborEventType {
    HELLO_RECEIVED = 0,
    TWOWAY_RECEIVED = 1,
    ONEWAY_RECEIVED = 2,
    KILL_NEIGHBOR = 3,
    INACTIVITY_TIMER = 4,
    LL_DOWN = 5, /*由下层协议说明邻居不可达*/
    EXCHANGE_DONE = 6,
    LOADING_DONE = 7,
    INVALID_HASH = 8

};
class NdnRoutingNeighbor;
class NdnRoutingNeighborState {
   public:
    NdnRoutingNeighborState(NdnRoutingNeighbor* _neighbor) : neighbor(_neighbor) {}
    virtual NeighborStateType getState() = 0;
    virtual void processEvent(NeighborEventType event) = 0;

   protected:
    NdnRoutingNeighbor* neighbor;
};
std::string getNameForNeighborEvent(NeighborEventType eventType);
std::string getNameForNeighborState(NeighborStateType stateType);
#endif