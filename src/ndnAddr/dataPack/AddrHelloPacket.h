#ifndef __ADDRHELLOPACKET_H_
#define __ADDRHELLOPACKET_H_
#include <cstdint>
#include <arpa/inet.h>

#include "ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"
class AddrHelloPacket : public Jsonfiable{
   public:
    uint32_t routerID;
    uint32_t leader;
    uint32_t interfaceNum;
    Ipv4Address addr;
    Ipv4Address mask;
    uint16_t helloInterval;

    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};
#endif