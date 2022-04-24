#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "RegisterDataPack.h"
using namespace std;

class RegisterDataPackTest : public ::testing::Test {
   protected:
};
TEST_F(RegisterDataPackTest, testEncodeAndDecode) {
    auto adj = make_shared<LsaDataPack>();
    adj->lsType = ADJ;
    adj->routerID = rand();
    adj->seqNum = rand();
    adj->lsAge = rand() % (0xffff);
    adj->numberOfLinks = rand() % 5;
    for (int i = 0; i < adj->numberOfLinks; i++) {
        NdnLink tmp;
        tmp.linkType = POINTTOPOINT_LINK;
        tmp.linkID = rand();
        tmp.linkData = rand();
        tmp.linkData = rand();
        tmp.linkDataMask = rand();
        tmp.linkCost = rand();
        adj->links.push_back(tmp);
    }

    auto rch = make_shared<LsaDataPack>();
    rch->lsType = RCH;
    rch->routerID = rand();
    rch->seqNum = rand();
    rch->lsAge = rand() % (0xffff);
    rch->numberOfLinks = rand() % 5;
    for (int i = 0; i < rch->numberOfLinks; i++) {
        NdnLink tmp;
        tmp.linkType = POINTTOPOINT_LINK;
        tmp.linkID = rand();
        tmp.linkData = rand();
        tmp.linkData = rand();
        tmp.linkDataMask = rand();
        tmp.linkCost = rand();
        rch->links.push_back(tmp);
    }

    RegisterDataPack oldPacket;
    oldPacket.adjLsa = adj;
    oldPacket.rchLsa = rch;

    auto tmp = oldPacket.encode();
    RegisterDataPack newPacket;
    // cout << tmp.first << endl;
    newPacket.decode(tmp.second.get(), tmp.first);

    ASSERT_NE(newPacket.adjLsa, nullptr);
    ASSERT_NE(newPacket.rchLsa, nullptr);

    ASSERT_EQ(newPacket.adjLsa->lsType, oldPacket.adjLsa->lsType);
    ASSERT_EQ(newPacket.adjLsa->routerID, oldPacket.adjLsa->routerID);
    ASSERT_EQ(newPacket.adjLsa->seqNum, oldPacket.adjLsa->seqNum);
    ASSERT_EQ(newPacket.adjLsa->lsAge, oldPacket.adjLsa->lsAge);
    ASSERT_EQ(newPacket.adjLsa->numberOfLinks, oldPacket.adjLsa->numberOfLinks);
    for (int i = 0; i < newPacket.adjLsa->numberOfLinks; i++) {
        ASSERT_EQ(newPacket.adjLsa->links[i].linkID, oldPacket.adjLsa->links[i].linkID);
        ASSERT_EQ(newPacket.adjLsa->links[i].linkData, oldPacket.adjLsa->links[i].linkData);
        ASSERT_EQ(newPacket.adjLsa->links[i].linkDataMask, oldPacket.adjLsa->links[i].linkDataMask);
        ASSERT_EQ(newPacket.adjLsa->links[i].linkCost, oldPacket.adjLsa->links[i].linkCost);
        ASSERT_EQ(newPacket.adjLsa->links[i].linkType, oldPacket.adjLsa->links[i].linkType);
    }

    ASSERT_EQ(newPacket.rchLsa->lsType, oldPacket.rchLsa->lsType);
    ASSERT_EQ(newPacket.rchLsa->routerID, oldPacket.rchLsa->routerID);
    ASSERT_EQ(newPacket.rchLsa->seqNum, oldPacket.rchLsa->seqNum);
    ASSERT_EQ(newPacket.rchLsa->lsAge, oldPacket.rchLsa->lsAge);
    ASSERT_EQ(newPacket.rchLsa->numberOfLinks, oldPacket.rchLsa->numberOfLinks);
    for (int i = 0; i < newPacket.rchLsa->numberOfLinks; i++) {
        ASSERT_EQ(newPacket.rchLsa->links[i].linkID, oldPacket.rchLsa->links[i].linkID);
        ASSERT_EQ(newPacket.rchLsa->links[i].linkData, oldPacket.rchLsa->links[i].linkData);
        ASSERT_EQ(newPacket.rchLsa->links[i].linkDataMask, oldPacket.rchLsa->links[i].linkDataMask);
        ASSERT_EQ(newPacket.rchLsa->links[i].linkCost, oldPacket.rchLsa->links[i].linkCost);
        ASSERT_EQ(newPacket.rchLsa->links[i].linkType, oldPacket.rchLsa->links[i].linkType);
    }
}