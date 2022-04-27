#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnRoutingAclRuleItemConditionPacketName.h"
using namespace std;
class NdnRoutingAclRuleItemConditionPacketNameTest : public ::testing::Test {
   protected:
};
TEST_F(NdnRoutingAclRuleItemConditionPacketNameTest, testparseRule1) {
    vector<vector<string>> testPoints = {
        {"packetName", "/name/tmp"},
        {"packetName", "/name"},
    };
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditionPacketName condition;
        int x = 0;
        bool ok = condition.parseRule(testPoints[i], x);
        ASSERT_EQ(ok, true);
        ASSERT_EQ(x, 2);
    }
}
TEST_F(NdnRoutingAclRuleItemConditionPacketNameTest, testparseRule2) {
    vector<vector<string>> testPoints = {{"packetNamde", "/name/tmp"}, {"packetName"}, {"1"}, {}};
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditionPacketName condition;
        int x = 0;
        bool ok = condition.parseRule(testPoints[i], x);
        ASSERT_EQ(ok, false);
        ASSERT_EQ(x, 0);
    }
}