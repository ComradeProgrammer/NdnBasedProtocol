#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "DeRegisterInterestPack.h"
using namespace std;
class DeRegisterInterestPackTest : public ::testing::Test {
   protected:
};
TEST_F(DeRegisterInterestPackTest, testEncodeAndDecode) {
    DeRegisterInterestPack old;
    //old.root = rand();

    auto tmp = old.encode();
    DeRegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    //ASSERT_EQ(old.root, newPacket.root);
}

TEST_F(DeRegisterInterestPackTest, testSignature) {
    string packetName = "/routing/lo/register/1/2/1648024944921";

    DeRegisterInterestPack old;
   // old.root = rand();

    auto tmp = old.encode();
    DeRegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    //ASSERT_EQ(old.root, newPacket.root);
}