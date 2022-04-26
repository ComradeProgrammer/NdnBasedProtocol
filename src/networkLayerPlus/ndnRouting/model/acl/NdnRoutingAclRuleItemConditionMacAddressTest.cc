#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnRoutingAclRuleItemConditionMacAddress.h"
using namespace std;
class NdnRoutingAclRuleItemConditionMacAddressTest : public ::testing::Test {
   protected:
};

TEST_F(NdnRoutingAclRuleItemConditionMacAddressTest, testparseRule1) {
   vector<vector<string>>testPoints={
      {"macAddress","ff:ff:ff:ff:ff:ff"},
      {"macAddress","01:23:45:67:89:ab"},
   };
   for(int i=0;i<testPoints.size();i++){
      NdnRoutingAclRuleItemConditionMacAddress condition;
      int x=0;
      bool ok=condition.parseRule(testPoints[i],x);
      ASSERT_EQ(ok,true);
      ASSERT_EQ(x,2);
   }
}
TEST_F(NdnRoutingAclRuleItemConditionMacAddressTest, testparseRule2) {
    vector<vector<string>>testPoints={
      {"macAddress","ff:ff:ff:ff:ff:fg"},
      {"macAddress","01:23:45:67:89:axb"},
      {"macAddress","01:23:45:67:89:ab:ff"},
      {"macAddress","01:23"},
      {"macAddress",":01:23:45:67:89:ab:"},
      {"macAddress","1:23:45:67:89:ab"},
      {"macAddress",""},
      {"macAddress"},
      {"MacAddress","ff:ff:ff:ff:ff:ff"},
      {"mac","ff:ff:ff:ff:ff:ff"},
      {"ff:ff:ff:ff:ff:ff"},
      {}
   };
   for(int i=0;i<testPoints.size();i++){
      NdnRoutingAclRuleItemConditionMacAddress condition;
      int x=0;
      bool ok=condition.parseRule(testPoints[i],x);
      ASSERT_EQ(ok,false);
      ASSERT_EQ(x,0);
   }
}