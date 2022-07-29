#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "BlockChain.h"

using namespace std;
class BlockChainTest : public ::testing::Test {
   protected:
};

TEST_F(BlockChainTest, test1) {
    BlockChain c;
    for (int i = 0; i < 10; i++) {
        string data = to_string(i) + to_string(i * i);
        c.generateNewBlock(data.c_str(), data.size());
    }

    ASSERT_EQ(c.verify(), true);
    Block fake;
    fake.index = 11;
    fake.hash = fake.calculateHash();
    c.chain.push_back(fake);
    ASSERT_EQ(c.verify(), false);
    c.chain.pop_back();

    Block fake2;
    fake2.index = 11;
    fake2.setData("abck", 5);
    fake2.prevHash = c.chain[10].hash;
    c.chain.push_back(fake2);
    ASSERT_EQ(c.verify(), false);
}

TEST_F(BlockChainTest, testPoW) {
    Block fake;
    fake.index = 11;
    fake.setData("abck", 5);
    fake.hash = fake.calculateHash();

    int nonce = proveOfWork(2, fake.hash);

    string tmp = fake.hash.toString() + to_string(nonce);
    BlockHash newHash = sha256(tmp.c_str(), tmp.size());
    string tmp2 = newHash.toString();

    ASSERT_EQ(tmp2[0], '0');
    ASSERT_EQ(tmp2[1], '0');
}