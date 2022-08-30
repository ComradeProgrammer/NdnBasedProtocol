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

struct BlockData {
    int index = 0;
    int dataSize = 0;
    BlockHash prevHash;
    BlockHash hash;
    char data[];
};

std::pair<int, std::unique_ptr<char[]>> encodeBlockChain(BlockChain* chain) {
    int size = 0;
    for (int i = 0; i < chain->chain.size(); i++) {
        size += sizeof(BlockData)+ chain->chain[i].dataSize;
    }
    char* buffer = new char[size];
    char* ptr = buffer;
    for (int i = 0; i < chain->chain.size(); i++) {
        BlockData* tmp = (BlockData*)ptr;
        tmp->index = chain->chain[i].index;
        tmp->dataSize = chain->chain[i].dataSize;
        tmp->prevHash = chain->chain[i].prevHash;
        tmp->hash = chain->chain[i].hash;
        memcpy(tmp->data, chain->chain[i].data, tmp->dataSize);
        ptr += sizeof(BlockData) + chain->chain[i].dataSize;
    }
    return {size, unique_ptr<char[]>(buffer)};
}

BlockChain decodeBlockChain(const char* data, int dataLength) {
    BlockChain newChain;
    const char* ptr = data;
    while (ptr < data + dataLength) {
        BlockData* tmp = (BlockData*)ptr;
        Block b;
        b.index = tmp->index;
        b.dataSize = tmp->index;
        b.hash = tmp->hash;
        b.prevHash = tmp->prevHash;
        b.setData(tmp->data, tmp->dataSize);
        newChain.chain.push_back(b);
        ptr += sizeof(BlockData) + b.dataSize;
    }
    return newChain;
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

