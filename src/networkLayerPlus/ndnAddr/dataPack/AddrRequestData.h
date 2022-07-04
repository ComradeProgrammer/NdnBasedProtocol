#ifndef __ADDR_REQUEST_DATA_H_
#define __ADDR_REQUEST_DATA_H_
#include "networkLayer/ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"
#include "util/util.h"
class AddrRequestData : public Jsonfiable {
   public:
    Ipv4Address startAddr;
    Ipv4Address mask;

   public:
    AddrRequestData() = default;
    AddrRequestData(const AddrRequestData&) = delete;
    ~AddrRequestData() = default;

    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};
#endif