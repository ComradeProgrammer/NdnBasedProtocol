#ifndef __ADDRREQINTEREST_H_
#define __ADDRREQINTEREST_H_
#include <cstdint>
#include <arpa/inet.h>
#include "util/printable/Jsonifiable.h"
class AddrReqInterest: public Jsonfiable{
    public:
    uint32_t num;

    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

#endif