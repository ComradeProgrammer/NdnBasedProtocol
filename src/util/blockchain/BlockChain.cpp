#include "BlockChain.h"
using namespace std;

Block BlockChain::generateNewBlock(const char* data, int size) {
    Block newBlock;
    newBlock.setIndex(chain.size());
    newBlock.setData(data, size);
    if (chain.size() != 0) {
        newBlock.setPrevHash(chain[chain.size() - 1].getHash());
    }
    newBlock.setHash(newBlock.calculateHash());
    chain.push_back(newBlock);
    return newBlock;
}

bool BlockChain::verify() {
    for (int i = 1; i < chain.size(); i++) {
        if (chain[i].getPrevHash() != chain[i - 1].getHash()) {
            return false;
        }
        if (chain[i].getHash() != chain[i].calculateHash()) {
            return false;
        }
    }
    return true;
}