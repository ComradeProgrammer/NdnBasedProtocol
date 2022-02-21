#ifndef __TRANSMITTER_H_
#define __TRANSMITTER_H_
#include <mutex>
#include <thread>
#include <unordered_map>

#include "linkLayer/EthernetPacket.h"
#include "networkLayer/NetworkLayerProtocol.h"
class Transmitter {
   public:
    Transmitter() = default;
    Transmitter(const Transmitter&) = delete;
    /**
     * @brief listen for the packet and call onReceiveEthernetPacket once a new packet is received. This funtion can
     * block the current thread.
     */
    virtual void listen() = 0;
    virtual void registerNetworkLayerProtocol(int id, std::shared_ptr<NetworkLayerProtocol> protocol);
    /**
     * @brief send a packet with the transmitter. This function is THREAD SAFE
     * @warning this funtion will require the lock of this object and release it
     * when returned.
     * @param interfaceID index of interface
     * @param packet packet you want to send
     * @return Returns the number of bytes sent, or -1 for errors
     * */
    virtual int sendPacket(int targetInterface, std::shared_ptr<EthernetPacket> packet) = 0;

   protected:
    /**
     * @brief find the registered protocol by protocol ID and notify the protocol by calling its
     * onReceiveEthernetPacket() method. This function is THREAD SAFE, i.e.， this funtion will try to gain the lock.
     * This function will be called by listen method.
     *
     * @param sourceInterface id of interface from which we received this packet
     * @param protocolId id of protocol in ethernet packet.
     */
    virtual void onReceiveEthernetPacket(int sourceInterface, int protocolId, std::shared_ptr<EthernetPacket> packet);

   protected:
    std::mutex lock;
    std::unordered_map<int, std::shared_ptr<NetworkLayerProtocol>> protocols;
};
#endif