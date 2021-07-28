
#include "RawSocket.h"

using namespace std;

RawSocket::RawSocket(std::shared_ptr<Logger> _logger) {
    if (_logger == nullptr) {
        // if no logger specified, use cout default logger
        logger = make_shared<Logger>();
    }
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

