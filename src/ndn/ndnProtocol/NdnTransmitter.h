#ifndef __NDNTRANSMITTER_H_
#define __NDNTRANSMITTER_H_
#include<functional>
#include<vector>
#include<mutex>
#include<unordered_map>
#include"ethernet/interface/NIC.h"
#include"util/log/Logger.h"
#include"ndn/ndnPacket/NdnPacket.h"
#include"ethernet/ethernetPacket/MacAddress.h"
#include"ethernet/RawSocket.h"
class NdnTransmitter{
    public:
    //default constructor
    NdnTransmitter(std::shared_ptr<Logger> log = nullptr);
    //this object is forbidden to be coppied, thus copy constructor is deleted explicitly
    NdnTransmitter(const NdnTransmitter& )=delete;
    //deconstructor
    virtual ~NdnTransmitter()=default;

    /**
     * @brief send out packet via specified interface and specified mac address. This function is THREAD SAFE
     * @warning this funtion will require the lock and release it when return
     * @param interfaceIndex index of interface
     * @param mac Destination macaddress
     * @param packet packet you want to send
    */
    void send(int interfaceIndex,MacAddress mac, std::shared_ptr<NdnPacket>packet);

    /**
     * @brief register the handler when we receive a packet. the handler will be immdeiately called when a packet is received.
     * @param handler with singature void(int ,MacAddress, std::shared_ptr<NdnPacket>). This handler MUST BE THREAD SAFE! 
    */
    void setOnReceivePacket(std::function<void(int interfaceIndex,MacAddress sourceMac, std::shared_ptr<NdnPacket> packet)>handler);


    void listen();


    private:
    std::shared_ptr<Logger> logger = nullptr;
    std::mutex lock;
    std::unordered_map<int,NIC>nicMap;
    std::shared_ptr<RawSocket>rawSocket;
    std::function<void(int interfaceIndex,MacAddress sourceMac, std::shared_ptr<NdnPacket> packet)>handler=nullptr;

};

#endif 