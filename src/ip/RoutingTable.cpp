#include "RoutingTable.h"
using namespace std;
using nlohmann::json;

mutex RoutingTable::classLock;
shared_ptr<RoutingTable> RoutingTable::staticPtr=nullptr;

shared_ptr<RoutingTable> RoutingTable::getRoutingTable(shared_ptr<Logger>logger){
    lock_guard<mutex>lock(classLock);
    if(staticPtr==nullptr){
        RoutingTable* tmp=new RoutingTable(logger);
        staticPtr=shared_ptr<RoutingTable>(tmp);
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
    //remove the first line and second line
    for(int i=2;i<lines.size();i++){
        if(lines[i]==""){
            continue;
        }
        RoutingTableItem item(lines[i]);
        item.setFromRoutingProtocol(false);
        table.push_back(item);
    }
}

bool RoutingTable::addRoutingTableItem(RoutingTableItem item){
    
    //route add -net 1.1.1.0/24 gw 192.168.50.1
    stringstream ss;
    ss<<"route add -net "<<item.getDestination().toString()<<"/"<<item.getMask().getPrefixLength()<<" gw "<<item.getNextHop().toString();
    logger->VERBOSE(ss.str());

    //check whether there is identical routing item
    for(auto i: table){
        if(i.getDestination()==item.getDestination() && i.getMask()==item.getMask()){
            logger->WARNING("route with identical destination and mask already exists");
            return false;
        }
    }

    auto res=runCmd(ss.str());
    if(res.first!=0){
        logger->ERRORF("failed to run %s, %d returned,output is %s",ss.str().c_str(),res.first,res.second.c_str());
        return false;
    }

    table.push_back(item);
    return true;
}

void RoutingTable::removeAllItem(){
    for(auto i: table){
        if(!i.isFromRoutingProtocol()){
            continue;
        }
        deleteRoutingTableItem(i);
    }
}




bool RoutingTable::deleteRoutingTableItem(RoutingTableItem item){
     stringstream ss;
    ss<<"route delete -net "<<item.getDestination().toString()<<"/"<<item.getMask().getPrefixLength();
    logger->VERBOSE(ss.str());

    vector<RoutingTableItem>::iterator itr;
    for(itr=table.begin();itr!=table.end();itr++){
        if(itr->getDestination()==item.getDestination()&& itr->getMask()==item.getMask()){
            break;
        }
    }
    if(itr==table.end()){
        logger->ERRORF("failed to find identical item %s",item.toString().c_str());
        return false;
    }


    auto res=runCmd(ss.str());
    if(res.first!=0){
        logger->ERRORF("failed to run %s, %d returned,output is %s",ss.str().c_str(),res.first,res.second.c_str());
        return false;
    }
    table.erase(itr);
    return true;
}


json RoutingTable::marshal()const{
    vector<json>res;
    for(auto i : table){
        res.push_back(i.marshal());
    }
    json j(res);
    return j;
}
