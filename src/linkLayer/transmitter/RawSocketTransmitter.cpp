#include "RawSocketTransmitter.h"
using namespace std;
RawSocketTransmitter::RawSocketTransmitter() {
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock == -1) {
        LOGGER->ERROR("Failed to create a raw socket");
    }
}

RawSocketTransmitter::~RawSocketTransmitter() {
    if (sock != -1) {
        close(sock);
    }
}

int RawSocketTransmitter::sendPacket(int interfaceID, std::shared_ptr<EthernetPacket> packet) {
    // lock this block, which is this function here.
    lock_guard<mutex> blockLock(lock);
    sockaddr_ll peerMacAddr;
    socklen_t clntAddrSize = sizeof(peerMacAddr);
    memset(&peerMacAddr, 0, sizeof(peerMacAddr));
    // no need to fill in the mac address because we use raw packet, and the
    // packet contains it.
    peerMacAddr.sll_ifindex = interfaceID;
    // copy the header
    auto header = packet->getHeader();
    memcpy(buffer, &(header), 14);
    memcpy(buffer + 14, packet->getData(), packet->getPacketSize() - 14);

    int res = sendto(sock, buffer, packet->getPacketSize(), 0, (sockaddr *)&peerMacAddr, clntAddrSize);
    if(res<=0){
        sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (sock == -1) {
            LOGGER->ERROR("Failed to create a raw socket");
        }
    }
    return res;
}

void RawSocketTransmitter::listen() {
    while (1) {
        sockaddr_ll peerMacAddr;
        socklen_t clntAddrSize = sizeof(peerMacAddr);
        memset(&peerMacAddr, 0, sizeof(peerMacAddr));

        auto len = recvfrom(sock, buffer, MTU, 0, (sockaddr *)&peerMacAddr, &clntAddrSize);
        if (len == -1) {
            LOGGER->ERROR("RawSocket::receivePacket recvfrom get return value -1");
            return;
        }
        shared_ptr<EthernetPacket> packet = make_shared<EthernetPacket>(buffer, len);

        // remove packets which actually come from this instance.
        auto nicMap = IOC->getNicManager()->getAllNicsInMap();
        bool duplicate = false;
        for (auto pair : nicMap) {
            if (pair.second->getMacAddress() == packet->getHeader().getSourceMacAddress()) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            onReceiveEthernetPacket(peerMacAddr.sll_ifindex, peerMacAddr.sll_protocol, packet);
        }
    }
}
