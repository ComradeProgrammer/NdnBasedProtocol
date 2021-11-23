#ifndef __ROUTINGTABLEITEM_H_
#define __ROUTINGTABLEITEM_H_
#include<regex>
#include"Ipv4Address.h"
#include "util/log/Logger.h"
class RoutingTableItem{
    public:
    RoutingTableItem(Ipv4Address _dest,Ipv4Address _mask,Ipv4Address _nextHop, std::shared_ptr<Logger>_logger=nullptr):destination(_dest),mask(_mask),nextHop(_nextHop){
        logger=Logger::getDefaultLoggerIfNull(_logger);
    }
    RoutingTableItem(std::string info,std::shared_ptr<Logger>_logger=nullptr);

    Ipv4Address getDestination(){return destination;}
    Ipv4Address getMask(){return mask;}
    Ipv4Address getNextHop(){return nextHop;}
    bool isFromRoutingProtocol(){return fromRoutingProtocol;}
    void setFromRoutingProtocol(bool v){fromRoutingProtocol=v;}
    
    private:
    Ipv4Address destination;
    Ipv4Address mask;
    Ipv4Address nextHop;
    bool fromRoutingProtocol=false;
    std::shared_ptr<Logger>logger;
     
};
#endif