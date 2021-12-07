#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnInterest.h"
using namespace std;
class NdnInterestTest : public ::testing::Test {
   protected:
};

TEST_F(NdnInterestTest, testInterestBasic) {
    NdnInterest tmp;
    tmp.setName("test");
    tmp.setMustBeFresh(true);
    tmp.setNonce(100);
    tmp.setHopLimit(false, 55);
    tmp.setApplicationParameters(11, "Helloworld");
    auto res = tmp.encode();

    auto newInterest = NdnInterest::decode(res.second.get());
    ASSERT_NE(newInterest, nullptr);
    ASSERT_EQ(newInterest->getCanBePrefix(), false);
    ASSERT_EQ(newInterest->getMustBeFresh(), true);
    ASSERT_EQ(newInterest->getHopLimit().first, false);
    ASSERT_EQ(newInterest->getHopLimit().second, 55);
    ASSERT_EQ(newInterest->getName(), "test");
    ASSERT_EQ(string(newInterest->getApplicationParameters().second.get()), "Helloworld");
}
