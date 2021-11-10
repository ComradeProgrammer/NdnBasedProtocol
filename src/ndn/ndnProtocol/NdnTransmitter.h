#ifndef __NDNTRANSMITTER_H_
#define __NDNTRANSMITTER_H_
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "ethernet/RawSocket.h"
#include "ethernet/ethernetPacket/MacAddress.h"
#include "ethernet/interface/NICManager.h"
#include "ndn/ndnPacket/NdnPacket.h"
#include "util/log/Logger.h"
// single instance design pattern
class NdnTransmitter {
   public:
    static std::shared_ptr<NdnTransmitter> getTransmitter(
        std::shared_ptr<Logger> log = nullptr);

   private:
    static std::mutex classStaticLock;
    static std::shared_ptr<NdnTransmitter> singleInstance;
    // default constructor
    NdnTransmitter(std::shared_ptr<Logger> log = nullptr);
    // this object is forbidden to be coppied, thus copy constructor is deleted
    // explicitly
    NdnTransmitter(const NdnTransmitter&) = delete;

   public:
    // deconstructor
    virtual ~NdnTransmitter() = default;

    /**
     * @brief send out packet via specified interface and specified mac address.
     * This function is THREAD SAFE
     * @warning this funtion will require the lock and release it when return
     * @param interfaceIndex index of interface
     * @param mac Destination macaddress
     * @param packet packet you want to send
     */
    void send(int interfaceIndex, MacAddress mac,
              std::shared_ptr<NdnPacket> packet);

    /**
     * @brief register the handler when we receive a packet. the handler will be
     * immdeiately called when a packet is received.
     * @param handler with singature void(int ,MacAddress,
     * std::shared_ptr<NdnPacket>). This handler MUST BE THREAD SAFE!
     */
    void setOnReceivePacket(
        std::function<void(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnPacket> packet)>
            handler);

    /**
     * @brief infinite loop, which will monitor the incoming packet and call the
     * handler.
     * @warning currently the call of handler will be synchronized, but in a
     * future version we may consider to make it asynchronized
     */
    void listen();

   private:
    std::shared_ptr<Logger> logger = nullptr;
    std::mutex lock;
    std::shared_ptr<RawSocket> rawSocket;
    std::function<void(int interfaceIndex, MacAddress sourceMac,
                       std::shared_ptr<NdnPacket> packet)>
        handler = nullptr;
};

#endif