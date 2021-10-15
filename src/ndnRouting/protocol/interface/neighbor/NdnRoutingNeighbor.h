#ifndef __NDNROUTINGNEIGHBOR_H_
#define __NDNROUTINGNEIGHBOR_H_
#include "ethernet/ethernetPacket/MacAddress.h"
#include "ip/Ipv4Address.h"
#include "util/log/Logger.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborState.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborStateDown.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborStateInit.h"
class NdnRoutingInterface;
class NdnRoutingNeighbor{
    public:
    NdnRoutingNeighbor(NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger = nullptr);
    ~NdnRoutingNeighbor()=default;

    //getter and setter of some attributes
    //protocol lock should have been attained 
    uint32_t getRouterID(){return routerID;}
    //protocol lock should have been attained 
    MacAddress getMacAddress(){return macAddress;}
    //protocol lock should have been attained 
    Ipv4Address getIpAddress(){return ipAddress;}
    //protocol lock should have been attained 
    Ipv4Address getIpMask(){return ipMask;}
    
    //protocol lock should have been attained 
    void setRouterID(uint32_t _routerID){routerID=_routerID;}
    //protocol lock should have been attained 
    void setMacAddress(MacAddress _macAddress){macAddress=_macAddress;}
    //protocol lock should have been attained 
    void setIpAddress(Ipv4Address _ip){ipAddress=_ip;}
    //protocol lock should have been attained 
    void setIpMask(Ipv4Address _ipMask){ipMask=_ipMask;}

    void processEvent(NeighborEventType e);


    //protocol lock should have been attained 
    void changeState(NeighborStateType state);

    private:
    NdnRoutingInterface* interface;//pointer to the Ndn interface object which this object belongs to
    std::shared_ptr<Logger> logger;

    uint32_t routerID;
    MacAddress macAddress;
    Ipv4Address ipAddress;
    Ipv4Address ipMask;

    std::shared_ptr<NdnRoutingNeighborState>state;


};
#endif
