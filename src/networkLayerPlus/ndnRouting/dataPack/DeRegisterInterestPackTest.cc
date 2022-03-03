#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "DeRegisterInterestPack.h"
class DeRegisterInterestPackTest : public ::testing::Test {
   protected:
};
TEST_F(DeRegisterInterestPackTest, testEncodeAndDecode) {
    DeRegisterInterestPack old;
    old.root = rand();

    auto tmp = old.encode();
    DeRegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    ASSERT_EQ(old.root, newPacket.root);
}