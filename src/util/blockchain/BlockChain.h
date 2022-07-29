#ifndef __BLOCK_CHAIN_H_
#define __BLOCK_CHAIN_H_
#include <iostream>
#include <vector>

#include "Block.h"
class BlockChain {
   private:
    std::vector<Block> chain;

   public:
    Block generateNewBlock(const char* data, int size);
    bool verify();
};
int proveOfWork(int difficulty, BlockHash oldHash);
#endif