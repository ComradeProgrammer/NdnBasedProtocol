#ifndef __NETWORK_LAYER_PROTOCOL_H_ 
#define __NETWORK_LAYER_PROTOCOL_H_ 
#include"linkLayer/EthernetPacket.h"
class NetworkLayerProtocol{
    public:
    virtual void onReceiveEthernetPacket(int sourceInterface, std::shared_ptr<EthernetPacket>)=0;
};
#endif