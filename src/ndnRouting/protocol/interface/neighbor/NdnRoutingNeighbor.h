#ifndef __NDNROUTINGNEIGHBOR_H_
#define __NDNROUTINGNEIGHBOR_H_
#include<list>
#include "ethernet/ethernetPacket/MacAddress.h"
#include "ip/Ipv4Address.h"
#include "util/log/Logger.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborState.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborStateDown.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborStateInit.h"
#include"ndnRouting/protocol/interface/neighbor/neighborState/NdnRoutingNeighborStateExchange.h"
#include"ndnRouting/dataPack/LinkStateDigest.h"
#include"ndnRouting/dataPack/DDInterestPack.h"
#include"ndnRouting/dataPack/LsaInterestPack.h"
#include"ndnRouting/dataPack/DDDataPack.h"
#include "ndn/ndnPacket/NdnData.h"
#include "ndn/ndnPacket/NdnInterest.h"
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
    NeighborStateType getState(){return state->getState();}
    
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
    //protocol lock should have been attained 
    void clear();
    //protocol lock should have been attained 
    void createDatabaseSummary();
    //protocol lock should have been attained 
    void sendDDInterest();
    void sendDDData(int requestIndex,std::string name);

    /**
     * @brief send a local lsa interest to fetch the specified lsa. Retransmission will start automatically if no lsa recevied in assigned time.protocol lock should have been attained 
     * 
     * @param digest 
     */
    //protocol lock should have been attained 
    void sendLocalLsaInterestWithRetransmission(LinkStateDigest digest);
    void cancelLsaInterestRequest(LinkStateDigest digest);

    //protocol lock should have been attained 
    void sendLocalLsaInterest(LinkStateDigest digest);
    //protocol lock should have been attained 
    void onReceiveDDInterset(std::shared_ptr<NdnInterest> interest);
    //protocol lock should have been attained 
    void onReceiveDDData(std::shared_ptr<NdnData> data);

    bool hasPendingLsaRequest(){return localLsaPendingRequestList.size()!=0;}


    private:
    NdnRoutingInterface* interface;//pointer to the Ndn interface object which this object belongs to
    std::shared_ptr<Logger> logger;

    uint32_t routerID;
    MacAddress macAddress;
    Ipv4Address ipAddress;
    Ipv4Address ipMask;

    std::shared_ptr<NdnRoutingNeighborState>state;

    //used in Exchange state
    int recvingIndex=0;
    int sendingIndex=0;
    std::vector<LinkStateDigest>databaseSummary;
    std::vector<DDDataPack>ddList;
    
    std::list<LinkStateDigest>localLsaPendingRequestList;
};
#endif
