#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_FACTORY_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_FACTORY_H_
#include "NdnRoutingAclData.h"
#include "NdnRoutingAclRuleItemConditionIntf.h"
#include "NdnRoutingAclRuleItemConditionMacAddress.h"
#include "NdnRoutingAclRuleItemConditionPacketName.h"
#include "NdnRoutingAclRuleItemConditionPacketType.h"
#include "NdnRoutingAclRuleItemConditionRouterID.h"
#include "util/acl/AclRuleItemConditionFactory.h"
class NdnRoutingAclRuleItemConditionFactory : public AclRuleItemConditionFactory<NdnRoutingAclData> {
   public:
    virtual std::vector<std::shared_ptr<AclRuleItemCondition<NdnRoutingAclData>>> parse(std::string s) override;
};
#endif