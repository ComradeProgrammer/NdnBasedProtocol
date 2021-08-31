#ifndef __NDNPROTOCOL_H_
#define __NDNPROTOCOL_H_
#include <memory>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "ndn/ndnPacket/NdnInterest.h"
#include "util/util.h"
class NdnProtocol {
   public:
    // default constructor
    NdnProtocol(std::shared_ptr<Logger> log = nullptr);
    // this object is forbidden to be coppied, thus copy constructor is deleted
    // explicitly
    NdnProtocol(const NdnProtocol&) = delete;
    // deconstructor
    virtual ~NdnProtocol() = default;

   private:
    // pipeline functions

    void onReceiveInterest(int interfaceIndex, MacAddress sourceMac,
                           std::shared_ptr<NdnInterest> interest);

   private:
    std::shared_ptr<Logger> logger = nullptr;
};

#endif