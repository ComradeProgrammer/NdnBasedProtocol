#ifndef __BLOCK_H_
#define __BLOCK_H_
#include <iomanip>
#include <sstream>
#include <string>

#include "BlockHash.h"
class BlockChain;
class Block {
   public:
    Block() = default;
    Block(const Block& b);
    ~Block();

    void setData(const char* data, int size);
    const char* getData() { return data; }
    int getDataSize() { return dataSize; }

    int getIndex() { return index; }
    void setIndex(int _index) { index = _index; }

    BlockHash getPrevHash() { return prevHash; }
    void setPrevHash(BlockHash h) { prevHash = h; }
    BlockHash getHash() { return hash; }
    void setHash(BlockHash h) { hash = h; }

    BlockHash calculateHash();
    friend std::pair<int, std::unique_ptr<char[]>> encodeBlockChain(BlockChain* chain);
    friend BlockChain decodeBlockChain(const char* data, int dataLength);

   private:
    int index = 0;
    char* data = nullptr;
    int dataSize = 0;
    BlockHash prevHash;
    BlockHash hash;
};

#endif