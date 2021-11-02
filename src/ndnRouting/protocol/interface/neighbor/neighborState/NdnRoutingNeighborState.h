#ifndef __NDNROUTINGNEIGHBORSTATE_H_
#define __NDNROUTINGNEIGHBORSTATE_H_
#include<memory>
#include "util/log/Logger.h"

enum  NeighborStateType
{
    DOWN_STATE = 0,
    INIT_STATE = 1,
    EXCHANGE_STATE =2,
    LOADING_STATE =4,
    FULL_STATE = 8
};
enum NeighborEventType
{
    HELLO_RECEIVED = 0,
    TWOWAY_RECEIVED = 1,
    ONEWAY_RECEIVED = 2,
    KILL_NEIGHBOR = 3,
    INACTIVITY_TIMER = 4,
    LL_DOWN =5, /*由下层协议说明邻居不可达*/
    EXCHANGE_DONE =6,
    LOADING_DONE =7,
    DD_RETRANSMISSION_TIMER =9,
    REQ_RETRANSMISSION_TIMER =10,
};
class NdnRoutingInterface;
class NdnRoutingNeighbor;
class NdnRoutingNeighborState{
    public:
    NdnRoutingNeighborState(NdnRoutingNeighbor* _neighbor,NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger = nullptr):neighbor(_neighbor),interface(_interface){
        logger=Logger::getDefaultLoggerIfNull(_logger);
    }

    virtual NeighborStateType getState()=0;
    virtual void processEvent(NeighborEventType event)=0;

    protected:
    NdnRoutingNeighbor* neighbor;
    NdnRoutingInterface* interface;
    std::shared_ptr<Logger> logger;

};

std::string getNameForNeighborEvent(NeighborEventType eventType);
std::string getNameForNeighborState(NeighborStateType stateType);


#endif