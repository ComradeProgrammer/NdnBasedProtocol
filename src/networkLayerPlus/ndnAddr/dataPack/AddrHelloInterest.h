#ifndef __ADDR_HELLO_INTEREST_H_
#define __ADDR_HELLO_INTEREST_H_
#include "networkLayer/ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"
#include "util/util.h"

class AddrHelloInterest : public Jsonfiable {
   public:
    RouterID routerID;
    RouterID leader;
    int32_t interfaceNum;
    Ipv4Address addr;
    Ipv4Address mask;
    int helloInterval;

   public:
    AddrHelloInterest() = default;
    ~AddrHelloInterest() = default;
    AddrHelloInterest(const AddrHelloInterest&) = delete;

    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};
#endif