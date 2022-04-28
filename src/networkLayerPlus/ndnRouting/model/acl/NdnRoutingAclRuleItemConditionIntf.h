#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_INTF_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_INTF_H_
#include "NdnRoutingAclData.h"
#include "util/acl/AclRuleItemCondition.h"
class NdnRoutingAclRuleItemConditionIntf : public AclRuleItemCondition<NdnRoutingAclData> {
   public:
    virtual bool parseRule(std::vector<std::string> line, int& x) override;
    virtual bool match(NdnRoutingAclData* item) override;

   private:
    int interfaceIndex;
};
#endif