#ifndef __BLOCK_CHAIN_HASH_H_
#define __BLOCK_CHAIN_HASH_H_
#include <openssl/sha.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class BlockHash {
   public:
    char hash[SHA256_DIGEST_LENGTH] = {0};

   public:
    BlockHash() = default;
    BlockHash(const BlockHash& b);
    bool operator==(const BlockHash& b) const;
    bool operator!=(const BlockHash& b) const { return !((*this) == b); }
    std::string toString();
};

BlockHash sha256(const char* data, int size);
#endif