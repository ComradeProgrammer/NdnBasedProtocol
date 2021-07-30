#ifndef __RAWSOCKET_H_
#define __RAWSOCKET_H_
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <unordered_map>

#include "ethernet/ethernetPacket/EthernetPacket.h"
#include "util/log/Logger.h"
#define MTU 1500
class RawSocket {
   public:
    RawSocket(std::shared_ptr<Logger> _logger = nullptr);
    ~RawSocket();

    std::pair<int, std::shared_ptr<EthernetPacket>> receivePacket(
        uint16_t protocol);
    int sendPacket(int interfaceID, std::shared_ptr<EthernetPacket> packet);

   private:
    int sock;
    char buffer[MTU];
    std::shared_ptr<Logger> logger;
};

#endif