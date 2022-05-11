#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "DDDataPack.h"
#include "util/traceback/traceback.h"

using namespace std;
class DDDataPackTest : public ::testing::Test {
   protected:
};

TEST_F(DDDataPackTest, testEncodeAndDecode) {
    DDDataPack oldPack;
    oldPack.neighbor = 17231181;
    oldPack.interfaceMTU = 1500;
    oldPack.numberOfDDPackets = 5;

    for (int i = 0; i < 5; i++) {
        LinkStateDigest digest;
        digest.linkStateType = RCH;
        digest.routerID = i * i;
        digest.sequenceNum = i + 2;
        digest.lsAge = i;
        oldPack.ls.push_back(digest);
    }

    auto resPair = oldPack.encode();

    DDDataPack newPack;
    newPack.decode(resPair.second.get(), resPair.first);
    ASSERT_EQ(newPack.neighbor, oldPack.neighbor);
    ASSERT_EQ(newPack.interfaceMTU, oldPack.interfaceMTU);
    ASSERT_EQ(newPack.numberOfDDPackets, oldPack.numberOfDDPackets);
    ASSERT_EQ(newPack.ls.size(), oldPack.ls.size());
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(newPack.ls[i].routerID, oldPack.ls[i].routerID);
        ASSERT_EQ(newPack.ls[i].linkStateType, oldPack.ls[i].linkStateType);
        ASSERT_EQ(newPack.ls[i].sequenceNum, oldPack.ls[i].sequenceNum);
        ASSERT_EQ(newPack.ls[i].lsAge, oldPack.ls[i].lsAge);
    }
}

TEST_F(DDDataPackTest, testEncodeAndDecode2) {
    // initSignalTraceback([](string traceback) { cout<<traceback; });

    DDDataPack oldPack;
    oldPack.neighbor = 17231181;
    oldPack.interfaceMTU = 1500;
    oldPack.numberOfDDPackets = 5;

    for (int i = 0; i < 5; i++) {
        LinkStateDigest digest;
        digest.linkStateType = RCH;
        digest.routerID = i * i;
        digest.sequenceNum = i + 2;
        digest.lsAge = i;
        oldPack.ls.push_back(digest);
    }

    auto resPair = oldPack.encode();

    DDDataPack newPack;
    newPack.decode(resPair.second.get(), resPair.first);
    ASSERT_EQ(newPack.neighbor, oldPack.neighbor);
    ASSERT_EQ(newPack.interfaceMTU, oldPack.interfaceMTU);
    ASSERT_EQ(newPack.numberOfDDPackets, oldPack.numberOfDDPackets);
    ASSERT_EQ(newPack.ls.size(), oldPack.ls.size());
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(newPack.ls[i].routerID, oldPack.ls[i].routerID);
        ASSERT_EQ(newPack.ls[i].linkStateType, oldPack.ls[i].linkStateType);
        ASSERT_EQ(newPack.ls[i].sequenceNum, oldPack.ls[i].sequenceNum);
        ASSERT_EQ(newPack.ls[i].lsAge, oldPack.ls[i].lsAge);
    }
}