#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "RegisterInterestPack.h"
using namespace std;
class RegisterInterestPackTest : public ::testing::Test {
   protected:
};

TEST_F(RegisterInterestPackTest, testEncodeAndDecode) {
    RegisterInterestPack old;
   // old.root = rand();
    old.adjSequenceNum = rand();
    auto tmp = old.encode();
    RegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
   // ASSERT_EQ(old.root, newPacket.root);
    ASSERT_EQ(old.adjSequenceNum, newPacket.adjSequenceNum);
}

TEST_F(RegisterInterestPackTest, testSignature) {
    string packetName = "/routing/local/register/1/2/1648024944921";

    RegisterInterestPack old;
    //old.root = rand();
    old.adjSequenceNum = rand();
    auto tmp = old.encode();
    RegisterInterestPack newPacket;
    newPacket.decode(tmp.second.get(), tmp.first);
    //ASSERT_EQ(old.root, newPacket.root);

    ASSERT_EQ(old.adjSequenceNum, newPacket.adjSequenceNum);
}