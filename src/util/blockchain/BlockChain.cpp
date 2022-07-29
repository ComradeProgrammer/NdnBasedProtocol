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

int proveOfWork(int difficulty, BlockHash oldHash) {
    srand(time(nullptr));
    while (1) {
        int nonce = rand();
        auto tmp = oldHash.toString() + to_string(nonce);
        BlockHash newHash = sha256(tmp.c_str(), tmp.size());
        string str = newHash.toString();
        bool ok = true;
        for (int i = 0; i < difficulty && i < str.size(); i++) {
            if (str[i] != '0') {
                ok = false;
                break;
            }
        }
        if (ok) {
            return nonce;
        }
    }
}
