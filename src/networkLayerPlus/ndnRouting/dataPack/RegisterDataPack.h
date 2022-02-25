#ifndef __REGISTER_DATA_PACK_H_
#define __REGISTER_DATA_PACK_H_
#include "PacketCommon.h"
#include "util/printable/Jsonifiable.h"

class RegisterDataPack : public Jsonfiable {
   public:
    bool ok;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

#endif