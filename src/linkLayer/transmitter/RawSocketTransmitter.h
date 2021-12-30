#ifndef __RAW_SOCKET_TRANSMITTER_H_
#define __RAW_SOCKET_TRANSMITTER_H_
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <mutex>
#include <unordered_map>
#include"ioc.h"
#include"util/declaration.h"
#include"Transmitter.h"
class RawSocketTransmitter: public Transmitter{
    public:
    RawSocketTransmitter();
    RawSocketTransmitter (RawSocketTransmitter&)=delete;
    ~RawSocketTransmitter();
    virtual void listen()override;
    virtual int sendPacket(int targetInterface,std::shared_ptr<EthernetPacket>packet)override;

    protected:
    int sock;
    char buffer[MTU];
};

#endif