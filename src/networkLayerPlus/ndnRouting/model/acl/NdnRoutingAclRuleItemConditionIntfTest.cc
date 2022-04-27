#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
#include "NdnRoutingAclRuleItemConditionIntf.h"
using namespace std;
class NdnRoutingAclRuleItemConditionIntfTest : public ::testing::Test {
   protected:
};

TEST_F(NdnRoutingAclRuleItemConditionIntfTest, testparseRule1) {
    vector<vector<string>> testPoints = {
        {"interface", "100"},
        {"interface", "1"},
    };
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditionIntf condition;
        int x = 0;
        bool ok = condition.parseRule(testPoints[i], x);
        ASSERT_EQ(ok, true);
        ASSERT_EQ(x, 2);
    }
}

TEST_F(NdnRoutingAclRuleItemConditionIntfTest, testparseRule2) {
    vector<vector<string>> testPoints = {{"interfaced", "100"}, {"interface", "1x"}, {"interface"}, {"1"}, {}};
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditionIntf condition;
        int x = 0;
        bool ok = condition.parseRule(testPoints[i], x);
        ASSERT_EQ(ok, false);
        ASSERT_EQ(x, 0);
    }
}

TEST_F(NdnRoutingAclRuleItemConditionIntfTest, testparseRule3) {
    vector<string> testString = {"interface", "45"};
    NdnRoutingAclRuleItemConditionIntf condition;
    int x = 0;
    condition.parseRule(testString, x);

    NdnRoutingAclData data1;
    data1.interfaceIndex = 45;
    bool res1 = condition.checkValidity(&data1);
    ASSERT_EQ(res1, true);

    NdnRoutingAclData data2;
    data2.interfaceIndex = 450;
    bool res2 = condition.checkValidity(&data2);
    ASSERT_EQ(res2, false);
}