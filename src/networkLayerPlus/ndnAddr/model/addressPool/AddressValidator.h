#ifndef __ADDRESS_VALIDATOR_H_
#define __ADDRESS_VALIDATOR_H_

#include <unordered_map>
#include <vector>

#include "ioc.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "util/blockchain/BlockChain.h"
#include "util/util.h"
class AddressValidator {
   public:
    void establishFromChainBlock(std::string s);
    void clear();

    bool checkAddress(Ipv4Address net, Ipv4Address mask);

   private:
    std::vector<Ipv4Address> addresses;
    std::vector<Ipv4Address> masks;
};

#endif