#ifndef __ROUTING_TABLE_H_
#define __ROUTING_TABLE_H_
#include <mutex>
#include <vector>

#include "RoutingTableItem.h"
#include "util/cmd/cmd.h"
#include "util/printable/Jsonifiable.h"
class RoutingTable {
   public:
    RoutingTable();
    bool addRoutingTableItem(RoutingTableItem item);
    bool deleteRoutingTableItem(RoutingTableItem item);
    void removeAllItem();
    nlohmann::json marshal() const;

   private:
    static std::mutex classLock;
    std::vector<RoutingTableItem> table;
};
#endif