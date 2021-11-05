#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here

#include "InfoInterestPack.h"
using namespace std;
class InfoInterestPackTest : public ::testing::Test {
   protected:
};

TEST_F(InfoInterestPackTest, testEncodeAndDecode) {
    InfoInterestPack info;
    info.infoType = INFO_REFRESH;
    info.src = 789465;
    for (int i = 0; i < 3; i++) {
        LinkStateDigest digest;
        digest.routerID = i * i * 78 + 44;
        digest.linkStateType = RCH;
        digest.sequenceNum = i * i * i * i + 456;
        digest.lsAge = i;
        info.ls.push_back(digest);
    }
    for (int i = 0; i < 4; i++) {
        info.neighbors.push_back(i * i * i * i);
    }

    auto res = info.encode();

    InfoInterestPack newInfo;
    newInfo.decode(res.second.get(), res.first);
    ASSERT_EQ(newInfo.infoType, info.infoType);
    ASSERT_EQ(newInfo.src, info.src);
    ASSERT_EQ(newInfo.ls.size(), info.ls.size());
    ASSERT_EQ(newInfo.neighbors.size(), info.neighbors.size());
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(newInfo.ls[i].routerID, info.ls[i].routerID);
        ASSERT_EQ(newInfo.ls[i].linkStateType, info.ls[i].linkStateType);
        ASSERT_EQ(newInfo.ls[i].sequenceNum, info.ls[i].sequenceNum);
        ASSERT_EQ(newInfo.ls[i].lsAge, info.ls[i].lsAge);
    }

    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(newInfo.neighbors[i], info.neighbors[i]);
    }
}