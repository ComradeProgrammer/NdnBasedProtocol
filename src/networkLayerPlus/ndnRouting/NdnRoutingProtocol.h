#ifndef __NDN_ROUTING_PROTOCOL_H_
#define __NDN_ROUTING_PROTOCOL_H_
#include<mutex>
#include"networkLayerPlus/NdnProtocolPlus.h"
#include"networkLayer/ndn/NdnProtocol.h"
class NdnRoutingProtocol:public NdnProtocolPlus{
    public:
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);

    void start();

    friend class Controller;
    private:
    std::shared_ptr<std::mutex>lock;

};
#endif