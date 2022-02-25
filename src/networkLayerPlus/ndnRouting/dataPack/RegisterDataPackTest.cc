#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "RegisterDataPack.h"
class RegisterDataPackTest : public ::testing::Test {
   protected:
};
TEST_F(RegisterDataPackTest, testEncodeAndDecode) {
    RegisterDataPack oldPacket;
    oldPacket.ok = true;
    auto tmp = oldPacket.encode();
    RegisterDataPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    ASSERT_EQ(newPacket.ok, true);
}
TEST_F(RegisterDataPackTest, testEncodeAndDecode2) {
    RegisterDataPack oldPacket;
    oldPacket.ok = false;
    auto tmp = oldPacket.encode();
    RegisterDataPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    ASSERT_EQ(newPacket.ok, false);
}
