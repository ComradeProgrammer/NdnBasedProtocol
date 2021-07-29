#ifndef __RAWSOCKET_H_
#define __RAWSOCKET_H_
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>

#include "util/log/Logger.h"
class RawSocket {
   public:
    RawSocket(std::shared_ptr<Logger> _logger = nullptr);
    ~RawSocket();

      private:
    int sock;
    std::shared_ptr<Logger> logger;
};

#endif