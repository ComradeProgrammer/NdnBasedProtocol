#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "RegisterInterestPack.h"
class RegisterInterestPackTest : public ::testing::Test {
   protected:
};
TEST_F(RegisterInterestPackTest, testEncodeAndDecode) {
    RegisterInterestPack oldPacket;
    for (int i = 0; i < 16; i++) {
        oldPacket.databaseHash[i] = rand() % 0xff;
    }
    int size = rand() % 10;
    for (int i = 0; i < size; i++) {
        oldPacket.roots.push_back(rand());
    }

    auto tmp = oldPacket.encode();
    RegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(oldPacket.databaseHash[i], newPacket.databaseHash[i]);
    }
    for (int i = 0; i < size; i++) {
        ASSERT_EQ(oldPacket.roots[i], newPacket.roots[i]);
    }
}