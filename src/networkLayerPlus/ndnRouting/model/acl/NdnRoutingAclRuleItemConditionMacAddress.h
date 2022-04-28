#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_MACADDRESS_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_MACADDRESS_H_
#include <regex>
#include <string>

#include "NdnRoutingAclData.h"
#include "util/acl/AclRuleItemCondition.h"
class NdnRoutingAclRuleItemConditionMacAddress : public AclRuleItemCondition<NdnRoutingAclData> {
   public:
    virtual bool parseRule(std::vector<std::string> line, int& x) override;
    virtual bool match(NdnRoutingAclData* item) override;

   private:
    MacAddress address;
};
#endif