#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include"NdnRoutingAclRuleItem.h"
#include"NdnRoutingAclRuleItemConditionFactory.h"
using namespace std;
class NdnRoutingAclRuleItemTest : public ::testing::Test {
   protected:
};
TEST_F(NdnRoutingAclRuleItemTest,test1){
    shared_ptr<NdnRoutingAclRuleItemConditionFactory>factory=make_shared<NdnRoutingAclRuleItemConditionFactory>();
    NdnRoutingAclRuleItem i(factory);
    i.parse("");
    i.checkValidity(new NdnRoutingAclData());
}