#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include "LsaInterestPack.h"
using namespace std;

class LsaInterestPackTest : public ::testing::Test {
   protected:
};

TEST_F(LsaInterestPackTest, testEncodeAndDecode) {
    LsaInterestPack oldPack;
    oldPack.routerID = 17231181;
    oldPack.lsType = ADJ;
    oldPack.sequenceNum = 56465;

    auto res = oldPack.encode();
    LsaInterestPack newPack;
    newPack.decode(res.second.get(), res.first);
    ASSERT_EQ(oldPack.routerID, newPack.routerID);
    ASSERT_EQ(oldPack.sequenceNum, newPack.sequenceNum);
    ASSERT_EQ(oldPack.lsType, newPack.lsType);
}