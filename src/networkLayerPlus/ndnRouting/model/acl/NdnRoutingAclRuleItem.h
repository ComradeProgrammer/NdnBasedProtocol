#ifndef __NDN_ROUTING_ACL_RULE_ITEM_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_H_
#include "NdnRoutingAclData.h"
#include "NdnRoutingAclRuleItemConditionFactory.h"
#include "util/acl/AclRuleItem.h"
class NdnRoutingAclRuleItem : public AclRuleItem<NdnRoutingAclData> {
   public:
    NdnRoutingAclRuleItem(std::shared_ptr<NdnRoutingAclRuleItemConditionFactory> _factory) : AclRuleItem<NdnRoutingAclData>(_factory) {}
};
#endif