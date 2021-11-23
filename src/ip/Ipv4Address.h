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
    uint32_t addr;

    void setIp(std::string);
    std::string toString()const;
    bool operator==(const Ipv4Address& i) const { return addr == i.addr; }
} __attribute__((__packed__));

#endif