#include "Block.h"
using namespace std;
Block::Block(const Block& b) {
    index = b.index;
    prevHash = b.prevHash;
    hash = b.hash;
    dataSize = b.dataSize;
    data = new char[dataSize];
    memcpy(data, b.data, dataSize);
}
Block::~Block() { delete[] data; }
Block& Block::operator=(const Block& b) {
    if (data != nullptr) {
        delete[] data;
    }
    index = b.index;
    prevHash = b.prevHash;
    hash = b.hash;
    dataSize = b.dataSize;
    data = new char[dataSize];
    memcpy(data, b.data, dataSize);
    
    return *this;
}
void Block::setData(const char* oldData, int size) {
    if (data != nullptr) {
        delete[] data;
    }
    dataSize = size;
    data = new char[dataSize];
    memcpy(data, oldData, size);
}

BlockHash Block::calculateHash() {
    BlockHash res;
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    string indexString = to_string(index);
    SHA256_Update(&sha256, indexString.c_str(), indexString.size());
    SHA256_Update(&sha256, prevHash.hash, SHA256_DIGEST_LENGTH);
    SHA256_Update(&sha256, data, dataSize);
    SHA256_Final((unsigned char*)(res.hash), &sha256);

    return res;
}
