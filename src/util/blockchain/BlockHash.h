#ifndef __BLOCK_CHAIN_HASH_H_
#define __BLOCK_CHAIN_HASH_H_
#include <openssl/sha.h>

#include <cstring>
class BlockHash {
   public:
    char hash[SHA256_DIGEST_LENGTH] = {0};

   public:
    BlockHash() = default;
    BlockHash(const BlockHash& b);
    bool operator==(const BlockHash& b) const;
    bool operator!=(const BlockHash& b) const { return !((*this) == b); }
};
#endif