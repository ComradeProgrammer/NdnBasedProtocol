
#include "RawSocket.h"

using namespace std;

RawSocket::RawSocket(std::shared_ptr<Logger> _logger) {
    logger = Logger::getDefaultLoggerIfNull(_logger);
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock == -1) {
        logger->ERROR("Failed to create a raw socket");
    }
}

RawSocket::~RawSocket() {
    if (sock != -1) {
        close(sock);
    }
}
