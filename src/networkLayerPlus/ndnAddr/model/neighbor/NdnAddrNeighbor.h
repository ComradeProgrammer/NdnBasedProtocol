#ifndef __NDN_ADDR_NEIGHBOR_H_
#define __NDN_ADDR_NEIGHBOR_H_
#include "ioc.h"
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "util/util.h"
class NdnAddrAssignmentProtocol;
class NdnAddrInterface;
class NdnAddrNeighbor {
   public:
    RouterID getRouterID() { return routerID; }
    void setRouterID(RouterID _routerID) { routerID = _routerID; }

    MacAddress getMacAddress() { return macAddress; }
    void setMacAddress(MacAddress addr) { macAddress = addr; }

    RouterID getNeighborLeader() { return neighborLeader; }
    void setNeighborLeader(RouterID _neighborLeader) { neighborLeader = _neighborLeader; }

    int getRouterDeadInterval() { return routerDeadInterval; }
    void setRouterDeadInterval(int interval) { routerDeadInterval = interval; }

    int gerInterfaceNum() { return interfaceNum; }
    void setInterfaceNum(int _interfaceNum) { interfaceNum = _interfaceNum; }

    bool getIsStateUp() { return isStateUp; }
    void setIsStateUp(int _isStateUp) { isStateUp = _isStateUp; }

   private:
    RouterID routerID;
    MacAddress macAddress;
    RouterID neighborLeader;
    int routerDeadInterval;
    int interfaceNum;

    bool isStateUp = false;

    NdnAddrInterface* interface;
    NdnAddrAssignmentProtocol* protocol;
};

#endif