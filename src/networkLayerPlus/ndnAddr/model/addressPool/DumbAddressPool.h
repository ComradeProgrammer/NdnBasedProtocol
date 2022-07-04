#ifndef _DUMB_ADDRESS_POOL_H_
#define _DUMB_ADDRESS_POOL_H_
#include "AddressPool.h"
class DumbAddressPool : public AddressPool{
   public:
    DumbAddressPool();
    virtual void addAddressBlock(Ipv4Address address, Ipv4Address mask);
    virtual std::pair<Ipv4Address, Ipv4Address> allocateAddress(int size);

   private:
    Ipv4Address ptr;
};
#endif