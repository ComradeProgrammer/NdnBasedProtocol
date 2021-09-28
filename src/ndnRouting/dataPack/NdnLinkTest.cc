#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include "NdnLink.h"
using namespace std;
class NdnLinkTest : public ::testing::Test {
   protected:
};

TEST_F(NdnLinkTest, testEncodeAndDecode) {
    NdnLink oldPack;
    oldPack.linkType = STUB_LINK;
    oldPack.linkID = 4856;
    oldPack.linkData = 47882;
    oldPack.linkDataMask = 741174;
    oldPack.linkCost = 23489;

    auto res = oldPack.encode();
    NdnLink newPack;
    newPack.decode(res.second.get(), res.first);
    ASSERT_EQ(newPack.linkType, oldPack.linkType);
    ASSERT_EQ(newPack.linkID, oldPack.linkID);
    ASSERT_EQ(newPack.linkData, oldPack.linkData);
    ASSERT_EQ(newPack.linkDataMask, oldPack.linkDataMask);
    ASSERT_EQ(newPack.linkCost, oldPack.linkCost);
}