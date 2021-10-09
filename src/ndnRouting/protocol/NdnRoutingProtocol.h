#ifndef __NDNROUTINGPROTOCOL_H_
#define __NDNROUTINGPROTOCOL_H_
#include<unordered_map>
#include "util/log/Logger.h"
#include"ndn/ndnProtocol/NdnProtocol.h"
#include"ndnRouting/protocol/interface/NdnRoutingInterface.h"
//singleton design pattern
class NdnRoutingProtocol{
    public:
    ~NdnRoutingProtocol()=default;
    void initialize();

    void lock();
    void unlock();

    /**
     * @brief Set the RouterID  lock NEED to be attained before called
     */
    void setRouterID(uint32_t rid){routerID=rid;}
    /**
     * @brief get the RouterID  lock NEED to be attained before called 
     */
    uint32_t getRouterID(){return routerID;}

    void onReceivePacket(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnPacket>packet);
    void sendPacket( MacAddress sourceMac,
                           std::shared_ptr<NdnPacket>packet );

    private:
    std::shared_ptr<Logger> logger;
    std::mutex syncLock;
    uint32_t routerID;

    std::unordered_map<int,std::shared_ptr<NdnRoutingInterface>>interfaces;

    //singleton design implementation.
    public:
    static std::shared_ptr<NdnRoutingProtocol>getNdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);
    private:
    NdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);
    static std::shared_ptr<NdnRoutingProtocol>protocol;
    static std::mutex staticLock;
};
#endif