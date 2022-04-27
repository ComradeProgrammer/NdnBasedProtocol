#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnRoutingAclRuleItem.h"
#include "NdnRoutingAclRuleItemConditionFactory.h"
using namespace std;
class NdnRoutingAclRuleItemTest : public ::testing::Test {
   protected:
};

TEST_F(NdnRoutingAclRuleItemTest, test1) {
    vector<string> testPoints = {
        "rule 0 permit packetName /tmp",
        "rule   100  permit   packetName   /tmp macAddress ff:ff:ff:ff:ff:ff packetType dd interest",
        "rule 200  deny  packetName /tmp",
    };
    vector<int> answer1 = {0, 100, 200};
    vector<bool> answer2 = {true, true, false};
    vector<int> answer3 = {1, 3, 1};

    for (int i = 0; i < testPoints.size(); i++) {
        shared_ptr<NdnRoutingAclRuleItemConditionFactory> factory = make_shared<NdnRoutingAclRuleItemConditionFactory>();
        NdnRoutingAclRuleItem item(factory);
        bool ok = item.parse(testPoints[i]);
        ASSERT_EQ(ok, true);
        ASSERT_EQ(answer1[i], item.ruleID);
        ASSERT_EQ(answer2[i], item.permit);
        ASSERT_EQ(answer3[i], item.conditions.size());
    }
}

TEST_F(NdnRoutingAclRuleItemTest, test2) {
    vector<string> testPoints = {
        "rule  permit packetName /tmp",
        "rule   1d00  permit   packetName   /tmp macAddress ff:ff:ff:ff:ff:ff packetType dd interest",
        "rule 200  allow  packetName /tmp",
        "rule 0 packetName /tmp",
        "rule 0 permit",
        "permit packetName /tmp",
    };

    for (int i = 0; i < testPoints.size(); i++) {
        shared_ptr<NdnRoutingAclRuleItemConditionFactory> factory = make_shared<NdnRoutingAclRuleItemConditionFactory>();
        NdnRoutingAclRuleItem item(factory);
        bool ok = item.parse(testPoints[i]);
        ASSERT_EQ(ok, false);
    }
}