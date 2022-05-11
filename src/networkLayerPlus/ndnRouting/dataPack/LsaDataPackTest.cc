#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "LsaDataPack.h"
class LsaDataPackTest : public ::testing::Test {
   protected:
};

TEST_F(LsaDataPackTest, testEncodeAndDecode) {
    LsaDataPack oldPacket;
    oldPacket.lsType = ADJ;
    oldPacket.routerID = (uint64_t)rand() * (uint64_t)rand();
    oldPacket.seqNum = 49965;
    oldPacket.lsAge = 78;
    oldPacket.numberOfLinks = 5;
    for (int i = 0; i < 4; i++) {
        NdnLink tmp;
        tmp.linkType = STUB_LINK;
        tmp.linkID = 78945;
        tmp.linkDataMask = 123456;
        tmp.linkCost = 7412669;
        oldPacket.links.push_back(tmp);
    }

    auto res = oldPacket.encode();
    LsaDataPack newPacket;
    newPacket.decode(res.second.get(), res.first);
    ASSERT_EQ(oldPacket.lsType, newPacket.lsType);
    ASSERT_EQ(oldPacket.routerID, newPacket.routerID);
    ASSERT_EQ(oldPacket.seqNum, newPacket.seqNum);
    ASSERT_EQ(oldPacket.links.size(), newPacket.links.size());
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(oldPacket.links[i].linkType, newPacket.links[i].linkType);
        ASSERT_EQ(oldPacket.links[i].linkID, newPacket.links[i].linkID);
        ASSERT_EQ(oldPacket.links[i].linkData, newPacket.links[i].linkData);
        ASSERT_EQ(oldPacket.links[i].linkDataMask, newPacket.links[i].linkDataMask);
        ASSERT_EQ(oldPacket.links[i].linkCost, newPacket.links[i].linkCost);
    }
}

TEST_F(LsaDataPackTest, testEncodeAndDecodeWithSignature) {
    LsaDataPack oldPacket;
    oldPacket.lsType = ADJ;
    oldPacket.routerID = (uint64_t)rand() * (uint64_t)rand();
    oldPacket.seqNum = 49965;
    oldPacket.lsAge = 78;
    oldPacket.numberOfLinks = 5;
    for (int i = 0; i < 4; i++) {
        NdnLink tmp;
        tmp.linkType = STUB_LINK;
        tmp.linkID = 78945;
        tmp.linkDataMask = 123456;
        tmp.linkCost = 7412669;
        oldPacket.links.push_back(tmp);
    }

    auto res = oldPacket.encode();
    LsaDataPack newPacket;
    newPacket.decode(res.second.get(), res.first);
    ASSERT_EQ(oldPacket.lsType, newPacket.lsType);
    ASSERT_EQ(oldPacket.routerID, newPacket.routerID);
    ASSERT_EQ(oldPacket.seqNum, newPacket.seqNum);
    ASSERT_EQ(oldPacket.links.size(), newPacket.links.size());
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(oldPacket.links[i].linkType, newPacket.links[i].linkType);
        ASSERT_EQ(oldPacket.links[i].linkID, newPacket.links[i].linkID);
        ASSERT_EQ(oldPacket.links[i].linkData, newPacket.links[i].linkData);
        ASSERT_EQ(oldPacket.links[i].linkDataMask, newPacket.links[i].linkDataMask);
        ASSERT_EQ(oldPacket.links[i].linkCost, newPacket.links[i].linkCost);
    }
}