#ifndef __ADDRESS_POOL_H_
#define __ADDRESS_POOL_H_
#include <unordered_map>
#include "networkLayer/ip/Ipv4Address.h"
class AddressPool{
    public:
    virtual void addAddressBlock(Ipv4Address address, Ipv4Address mask)=0;
    virtual std::pair<Ipv4Address, Ipv4Address>allocateAddress(int size)=0;
    virtual void returnAddress(Ipv4Address, Ipv4Address)=0;
};
#endif