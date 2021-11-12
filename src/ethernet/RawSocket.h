#ifndef __RAWSOCKET_H_
#define __RAWSOCKET_H_
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "ethernet/ethernetPacket/EthernetPacket.h"
#include "util/declaration.h"
#include "util/log/Logger.h"
class RawSocket {
   public:
    RawSocket(std::shared_ptr<Logger> _logger = nullptr);
    virtual ~RawSocket();
    RawSocket(const RawSocket&) = delete;
    /**
     * @brief wait until the raw socket receive a packet of the specified
     * protocol. This function will jam the current thread until a packet is
     * received.
     * @return std::pair<int, std::shared_ptr<EthernetPacket>>,
     * - 1st value: index of the interface which receives the packet
     * - 2nd value: the packet
     * @param protocol: the protocol number
     */
    std::pair<int, std::shared_ptr<EthernetPacket>> receivePacket(uint16_t protocol);

    /**
     * @brief send a packet with the raw socket. This function is THREAD SAFE
     * @warning this funtion will require the lock of this object and release it
     * when returned.
     * @param interfaceID index of interface
     * @param packet packet you want to send
     * @return Returns the number of bytes sent, or -1 for errors
     * */
    int sendPacket(int interfaceID, std::shared_ptr<EthernetPacket> packet);

   private:
    int sock;
    char buffer[MTU];
    std::shared_ptr<Logger> logger;
    std::mutex lock;
};

#endif