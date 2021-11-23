#ifndef __ROUTINGTABLE_H_
#define __ROUTINGTABLE_H_
#include<vector>
#include<mutex>
#include"util/cmd/cmd.h"
#include"RoutingTableItem.h"
//singleton pattern
class RoutingTable{
    public:
    static std::shared_ptr<RoutingTable> getRoutingTable(std::shared_ptr<Logger>logger=nullptr);

    private:
    static std::mutex classLock;
    static std::shared_ptr<RoutingTable> staticPtr;  
    RoutingTable(std::shared_ptr<Logger>logger=nullptr);
    std::vector<RoutingTableItem>table;
    std::shared_ptr<Logger>logger;
};
#endif