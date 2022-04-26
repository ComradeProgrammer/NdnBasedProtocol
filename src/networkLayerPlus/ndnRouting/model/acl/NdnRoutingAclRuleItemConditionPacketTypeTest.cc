#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnRoutingAclRuleItemConditionPacketType.h"
using namespace std;
class NdnRoutingAclRuleItemConditionPacketTypeTest : public ::testing::Test {
   protected:
};

TEST_F(NdnRoutingAclRuleItemConditionPacketTypeTest, testparseRule1) {
    vector<vector<string>> testPoints = {
        {"packetType", "hello", "interest"}, {"packetType", "dd", "interest"},       {"packetType", "lsa", "interest"},
        {"packetType", "info", "interest"},  {"packetType", "register", "interest"}, {"packetType", "deregister", "interest"},
        {"packetType", "hello", "data"},     {"packetType", "dd", "data"},           {"packetType", "lsa", "data"},
        {"packetType", "info", "data"},      {"packetType", "register", "data"},     {"packetType", "deregister", "data"},
    };
    vector<PacketKind> answer1 = {HELLO, DD, LSA, INFO, REGISTER, DEREGISTER, HELLO, DD, LSA, INFO, REGISTER, DEREGISTER};
    vector<PacketType> answer2 = {INTEREST, INTEREST, INTEREST, INTEREST, INTEREST, INTEREST, DATA, DATA, DATA, DATA, DATA, DATA};
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditionPacketType condition;
        int pos = 0;
        condition.parseRule(testPoints[i], pos);

        ASSERT_EQ(condition.packetKind, answer1[i]);
        ASSERT_EQ(condition.packetType, answer2[i]);
        ASSERT_EQ(pos, 3);
    }
}
TEST_F(NdnRoutingAclRuleItemConditionPacketTypeTest, testparseRule2) {
    vector<vector<string>> testPoints = {
        {"packettype", "hello", "interest"},
        {"packetType", "d", "interest"},
        {"packetType", "la", "interest"},
        {"packetType", "info", "inteest"},
        {"packetType", "interest"},
        {"dergister", "interest"},
        {"packetType", "hello"},
        {
            "packetType",
        },
        {},
    };
    for (int i = 0; i < testPoints.size(); i++) {
        NdnRoutingAclRuleItemConditionPacketType condition;
        int pos = 0;
        bool ok = condition.parseRule(testPoints[i], pos);
        ASSERT_EQ(ok, false);
        ASSERT_EQ(pos, 0);
    }
}