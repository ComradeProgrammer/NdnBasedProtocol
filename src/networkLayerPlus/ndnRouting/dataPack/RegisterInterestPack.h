#ifndef __REGISTER_INTEREST_PACK_H_
#define __REGISTER_INTEREST_PACK_H_
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"

class RegisterInterestPack : public Jsonfiable {
   public:
    unsigned char databaseHash[16];
    /**
     * @brief when A send a RegisterInterestPack to B with roots=[1,3], that means A register B as parent in minimum-hop trees whose roots are 1,3
     */
    std::vector<RouterID> roots;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};
#endif