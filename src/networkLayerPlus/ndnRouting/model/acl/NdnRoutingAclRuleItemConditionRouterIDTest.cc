#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnRoutingAclRuleItemConditionRouterID.h"
using namespace std;
class NdnRoutingAclRuleItemConditionRouterIDTest : public ::testing::Test {
   protected:
};

TEST_F(NdnRoutingAclRuleItemConditionRouterIDTest, testparseRule1) {
    vector<vector<string>> testPoints = {
        {"routerID", "0"},
        {"routerID", "6271792508405061783"},
    };
    vector<RouterID> answer1 = {0, 6271792508405061783};
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditiionRouterID condition;
        int pos = 0;
        bool ok = condition.parseRule(testPoints[i], pos);
        ASSERT_EQ(ok, true);
        ASSERT_EQ(answer1[i], condition.routerID);
        ASSERT_EQ(pos, 2);
    }
}

TEST_F(NdnRoutingAclRuleItemConditionRouterIDTest, testparseRule2) {
    vector<vector<string>> testPoints = {{"routerid", "0"}, {"routerID"}, {}};
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditiionRouterID condition;
        int pos = 0;
        bool ok = condition.parseRule(testPoints[i], pos);
        ASSERT_EQ(ok, false);
        ASSERT_EQ(pos, 0);
    }
}

TEST_F(NdnRoutingAclRuleItemConditionRouterIDTest, testparseRule3) {
    vector<string> testString = {"routerID", "6271792508405061783"};
    NdnRoutingAclRuleItemConditiionRouterID condition;
    int x = 0;
    condition.parseRule(testString, x);

    NdnRoutingAclData data1;
    data1.sourceRouterID = 6271792508405061783;
    bool res1 = condition.checkValidity(&data1);
    ASSERT_EQ(res1, true);

    NdnRoutingAclData data2;
    data2.sourceRouterID = 450;
    bool res2 = condition.checkValidity(&data2);
    ASSERT_EQ(res2, false);
}