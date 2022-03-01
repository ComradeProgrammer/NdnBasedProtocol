#ifndef __REGISTER_DATA_PACK_H_
#define __REGISTER_DATA_PACK_H_
#include "LsaDataPack.h"
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"
class RegisterDataPack : public Jsonfiable {
   public:
    std::shared_ptr<LsaDataPack> adjLsa=nullptr;
    std::shared_ptr<LsaDataPack> rchLsa=nullptr;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

#endif