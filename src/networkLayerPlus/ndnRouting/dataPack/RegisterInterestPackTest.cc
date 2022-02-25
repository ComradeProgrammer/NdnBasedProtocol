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
    RegisterInterestPack  old;
    old.root=rand();
    old.linkStateType=LinkStateType::ADJ;
    old.sequenceNum=rand();

    auto tmp=old.encode();
    RegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(),tmp.first);
    ASSERT_EQ(old.root,newPacket.root);
    ASSERT_EQ(old.linkStateType,newPacket.linkStateType);
    ASSERT_EQ(old.sequenceNum,newPacket.sequenceNum);

}