#ifndef __NDN_ROUTING_NEIGHBOR_STATE_H_
#define __NDN_ROUTING_NEIGHBOR_STATE_H_
enum NeighborStateType { DOWN = 0, INIT = 1, EXCHANGE = 2, LOADING = 4, FULL = 8 };
enum NeighborEventType {
    HELLO_RECEIVED = 0,
    TWOWAY_RECEIVED = 1,
    ONEWAY_RECEIVED = 2,
    KILL_NEIGHBOR = 3,
    INACTIVITY_TIMER = 4,
    LL_DOWN = 5, /*由下层协议说明邻居不可达*/
    EXCHANGE_DONE = 6,
    LOADING_DONE = 7,
    DD_RETRANSMISSION_TIMER = 9,
    REQ_RETRANSMISSION_TIMER = 10,
};
class NdnRoutingNeighbor;
class NdnRoutingNeighborState{
    public:
    NdnRoutingNeighborState(NdnRoutingNeighbor* _neighbor):neighbor(_neighbor){}
    virtual NeighborStateType getState() = 0;
    virtual void processEvent(NeighborEventType event) = 0;
    private:
    NdnRoutingNeighbor* neighbor;

};
#endif 