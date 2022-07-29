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
std::string BlockHash::toString() {
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << setw(2) << setfill('0') << hex << ((int)(hash[i]));
    }
    return ss.str();
}

BlockHash sha256(const char* data, int size) {
    BlockHash newHash;
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, size);
    SHA256_Final((unsigned char*)(newHash.hash), &sha256);
    return newHash;
}