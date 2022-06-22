#ifndef __ADDR_HELLO_INTEREST_H_
#define __ADDR_HELLO_INTEREST_H_
#include "util/util.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"

class AddrHelloInterest: public Jsonfiable{
    public:
    RouterID routerID;
    RouterID leader;
    int32_t requiredSize;
    Ipv4Address addr;
    Ipv4Address mask;

    public:
    AddrHelloInterest()=default;
    ~AddrHelloInterest()=default;
    AddrHelloInterest(const AddrHelloInterest&)=delete;

    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;

};
#endif