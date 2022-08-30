#ifndef __BLOCK_CHAIN_H_
#define __BLOCK_CHAIN_H_
#include <iostream>
#include <vector>

#include "Block.h"
class BlockChain {
   public:
    std::vector<Block> chain;

   public:
    Block generateNewBlock(const char* data, int size);
    bool verify();
};

std::pair<int, std::unique_ptr<char[]>> encodeBlockChain(BlockChain* chain);
BlockChain decodeBlockChain(const char* data, int dataLength);
int proveOfWork(int difficulty, BlockHash oldHash);

#endif