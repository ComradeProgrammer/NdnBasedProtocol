#ifndef __INFOINTERESTPACK_H_
#define __INFOINTERESTPACK_H_
#include <cstdint>
#include <vector>

#include "LinkStateDigest.h"
enum InfoType { DOWN = 0, UP = 1, REFRESH = 2 };
class InfoInterestPack {
   public:
    InfoType infoType;
    uint32_t src;
    std::vector<LinkStateDigest> ls;
    std::vector<uint32_t> neighbors;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
};
#endif