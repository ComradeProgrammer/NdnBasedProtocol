#ifndef __HELLOINTERESTPACK_H_
#define __HELLOINTERESTPACK_H_
#include <arpa/inet.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "ip/Ipv4Address.h"
#include "PacketCommon.h"
class HelloInterestPack :public Printable{
   public:
    uint32_t routerId;
    Ipv4Address interfaceIP;
    Ipv4Address networkMask;
    uint16_t helloInterval;
    uint16_t routerDeadInterval;
    std::vector<Ipv4Address> neighbor;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual std::string toString() override;

};

#endif