#include "BlockHash.h"

using namespace std;
BlockHash::BlockHash(const BlockHash& b) { memcpy(hash, b.hash, SHA256_DIGEST_LENGTH); }

bool BlockHash::operator==(const BlockHash& b) const {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        if (hash[i] != b.hash[i]) {
            return false;
        }
    }
    return true;
}