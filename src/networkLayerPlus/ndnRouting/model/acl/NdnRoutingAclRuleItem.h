#ifndef __NDN_ROUTING_ACL_RULE_ITEM_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_H_
#include"util/acl/AclRuleItem.h"
#include"NdnRoutingAclData.h"
#include "NdnRoutingAclRuleItemConditionFactory.h"
class NdnRoutingAclRuleItem: public AclRuleItem<NdnRoutingAclData>{
    public:
    NdnRoutingAclRuleItem(std::shared_ptr<NdnRoutingAclRuleItemConditionFactory>_factory): AclRuleItem<NdnRoutingAclData>(_factory){}
};
#endif