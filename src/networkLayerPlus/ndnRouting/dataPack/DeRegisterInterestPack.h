#ifndef __DEREGISTER_INTEREST_PACK_H_
#define __DEREGISTER_INTEREST_PACK_H_
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"

class DeRegisterInterestPack : public Jsonfiable {
   public:
    RouterID root;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

#endif