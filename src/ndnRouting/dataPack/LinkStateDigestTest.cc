#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include "LinkStateDigest.h"

using namespace std;
class LinkStateDigestTest : public ::testing::Test {
   protected:
};

TEST_F(LinkStateDigestTest, testEncodeAndDecode) {
    LinkStateDigest digest;
    digest.routerID = 1000525;
    digest.linkStateType = LinkStateType::ADJ;
    digest.sequenceNum = 485;
    digest.lsAge = 10;

    auto tmp = digest.encode();
    LinkStateDigest newDigest;
    newDigest.decode(tmp.second.get(), tmp.first);
    ASSERT_EQ(digest.routerID, newDigest.routerID);
    ASSERT_EQ(digest.linkStateType, newDigest.linkStateType);
    ASSERT_EQ(digest.sequenceNum, newDigest.sequenceNum);
    ASSERT_EQ(digest.lsAge, newDigest.lsAge);
}