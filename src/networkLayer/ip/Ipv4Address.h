#ifndef __IPV4Address_H_
#define __IPV4Address_H_
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdint>
#include <cstring>
#include <string>
struct Ipv4Address {
    // stored in network sequence
    Ipv4Address() = default;
    Ipv4Address(std::string ip) { setIp(ip); }
    uint32_t addr;

    void setIp(std::string);
    std::string toString() const;
    bool operator==(const Ipv4Address& i) const { return addr == i.addr; }
    Ipv4Address andMask(Ipv4Address mask);
    int getPrefixLength();
} __attribute__((__packed__));

#endif