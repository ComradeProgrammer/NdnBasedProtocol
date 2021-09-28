#ifndef __DDINTERESTPACK_H_
#define __DDINTERESTPACK_H_
#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
class DDInterestPack {
   public:
    uint32_t neighbor;
    // uint16_t idx;//this field can be retrived through name of packet

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
};

#endif