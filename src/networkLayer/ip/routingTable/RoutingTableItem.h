#ifndef __ROUTINGTABLE_ITEM_H_
#define __ROUTINGTABLE_ITEM_H_
#include <regex>
#include <string>

#include "networkLayer/ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"
class RoutingTableItem : public Jsonfiable {
   public:
    RoutingTableItem(Ipv4Address _dest, Ipv4Address _mask, Ipv4Address _nextHop) : mask(_mask), nextHop(_nextHop) { destination = _dest.andMask(_mask); }
    RoutingTableItem(std::string info);

    Ipv4Address getDestination() { return destination; }
    Ipv4Address getMask() { return mask; }
    Ipv4Address getNextHop() { return nextHop; }
    bool isFromRoutingProtocol() { return fromRoutingProtocol; }
    void setFromRoutingProtocol(bool v) { fromRoutingProtocol = v; }
    nlohmann::json marshal() const;

   private:
    Ipv4Address destination;
    Ipv4Address mask;
    Ipv4Address nextHop;
    bool fromRoutingProtocol = false;
};
#endif