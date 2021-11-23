#include "RoutingTable.h"
using namespace std;
mutex RoutingTable::classLock;
shared_ptr<RoutingTable> RoutingTable::staticPtr=nullptr;

shared_ptr<RoutingTable> RoutingTable::getRoutingTable(shared_ptr<Logger>logger){
    lock_guard<mutex>lock(classLock);
    if(staticPtr==nullptr){
        RoutingTable* tmp=new RoutingTable(logger);
        staticPtr=shared_ptr<RoutingTable>(staticPtr);
    }
    return staticPtr;
}
RoutingTable::RoutingTable(shared_ptr<Logger>_logger){
    logger=Logger::getDefaultLoggerIfNull(_logger);
    auto res=runCmd("route -n");
    if(res.first!=0){
        logger->ERRORF("failed to run route -n: return value %d",res.first);
        return;
    }
    auto lines=split(res.second,"\n");
    if(lines.size()<2){
         logger->ERROR("got less than 2 lines output from command 'route -n'");
         return;
    }
    for(int i=2;i<lines.size();i++){
        RoutingTableItem item(lines[i]);
        item.setFromRoutingProtocol(false);
        table.push_back(item);
    }
}

