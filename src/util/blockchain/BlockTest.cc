#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "Block.h"

using namespace std;
class BlockTest : public ::testing::Test {
   protected:
};

TEST_F(BlockTest, test1) {
    Block block1;
    block1.setIndex(0);
    const char* tmp = "Hello World";
    block1.setData(tmp, strlen(tmp));
    block1.hash = block1.calculateHash();

    Block block2 = block1;
    bool ok = (block2.calculateHash() == block2.getHash());
    ASSERT_EQ(ok, true);
    bool ok2 = (block2.calculateHash() == block1.getHash());
    ASSERT_EQ(ok2, true);

    Block block3;
    block3.setIndex(1);
    const char* tmp3 = "Hello World";
    block3.setData(tmp3, strlen(tmp));
    block3.hash = block3.calculateHash();

    bool ok3 = (block3.calculateHash() == block1.getHash());
    ASSERT_EQ(ok3, false);

    Block block4;
    block4.setIndex(1);
    const char* tmp4 = "HelloWorld";
    block4.setData(tmp4, strlen(tmp));
    block4.hash = block4.calculateHash();

    bool ok4 = (block4.calculateHash() == block1.getHash());
    ASSERT_EQ(ok4, false);
}