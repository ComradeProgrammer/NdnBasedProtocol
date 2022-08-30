#ifndef __NDN_ADDR_INTERFACE_H_
#define __NDN_ADDR_INTERFACE_H_
#include <string>

#include "ioc.h"
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceState.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateDown.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateLeader.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateNormal.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateWaiting.h"
#include "networkLayerPlus/ndnAddr/model/neighbor/NdnAddrNeighbor.h"
#include "physicalLayer/nic/NicObserverInterface.h"
#include "util/cmd/cmd.h"
class NdnAddrAssignmentProtocol;
class NdnAddrInterface : public NicObserverInterface {
   public:
    NdnAddrInterface(NdnAddrAssignmentProtocol* _protocol);
    std::string getName() { return name; }
    void setName(std::string _name) { name = _name; }

    int getInterfaceID() { return interfaceID; }
    void setInterfaceID(int _interfaceID) { interfaceID = _interfaceID; }

    MacAddress getMacAddress() { return macAddress; }
    void setMacAddress(MacAddress address) { macAddress = address; }

    Ipv4Address getIpv4Address() { return ipv4Addr; }
    void setIpv4Address(Ipv4Address addr) { ipv4Addr = addr; }

    Ipv4Address getAddrBlock() { return addrBlock; }
    void setAddrBlock(Ipv4Address block) { addrBlock = block; }

    Ipv4Address getIpv4Mask() { return ipv4Mask; }
    void setIpv4Mask(Ipv4Address mask) { ipv4Mask = mask; }

    NdnAddrInterfaceStateType getStateType() { return state->getState(); }
    void setState(NdnAddrInterfaceStateType stateType);

    void setLeader(RouterID id) { leader = id; }
    RouterID getLeader() { return leader; }

    bool getAddrAssigned() { return addrAssigned; }
    void setAddrAssigned(bool v) { addrAssigned = v; }
    void setNextAddr(Ipv4Address addr){nextAddr=addr;} 

    NdnAddrAssignmentProtocol* getProtocol() { return protocol; }

    // implementing NicObserverInterface
    virtual void onEventHappen(int interfaceID, NICEvent event) override;
    void processInterfaceEvent(NdnAddrInterfaceEventType event);

    void addNeighbor(std::shared_ptr<NdnAddrNeighbor> neighbor) { neighbors[neighbor->getRouterID()] = neighbor; }
    std::shared_ptr<NdnAddrNeighbor> getNeighborByRouterID(RouterID id);
    int getNeighborNum();

    RouterID calculateLeader();
    void sendBroadcastAddrRequest();
    void sendLocalAddrRequest();
    void syncIpAddress();

    Ipv4Address leaderAssignNextAddr();
    //routerid->分配的地址
    std::unordered_map<RouterID,Ipv4Address>assignment;

   private:
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    //自己的地址
    Ipv4Address ipv4Addr;
    //获得的地址块（如果是leader）
    Ipv4Address addrBlock;
    //掩码
    Ipv4Address ipv4Mask;
    //下一个分配出去的地址块
    Ipv4Address nextAddr;
    // routerID=0 means an non-existing router
    RouterID leader = 0;
    bool addrAssigned = false;
    std::unordered_map<RouterID, std::shared_ptr<NdnAddrNeighbor>> neighbors;
    NdnAddrAssignmentProtocol* protocol;
    std::shared_ptr<NdnAddrInterfaceState> state = nullptr;
};
#endif