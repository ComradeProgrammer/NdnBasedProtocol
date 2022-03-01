#ifndef __REGISTER_DATA_PACK_H_
#define __REGISTER_DATA_PACK_H_
#include "LsaDataPack.h"
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"
class RegisterDataPack : public Jsonfiable {
    // when adjLsaSize==0 && rchLsaSize==0 then that means registration is successful
   public:
    std::shared_ptr<LsaDataPack> adjLsa;
    std::shared_ptr<LsaDataPack> rchLsa;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

#endif