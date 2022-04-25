#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_FACTORY_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_FACTORY_H_
#include"util/acl/AclRuleItemConditionFactory.h"
#include"NdnRoutingAclData.h"
class NdnRoutingAclRuleItemConditionFactory:public AclRuleItemConditionFactory<NdnRoutingAclData>{
    public:
    virtual std::vector<std::shared_ptr<AclRuleItemCondition<NdnRoutingAclData>>> parse(std::string s) override{
        return {};
    }
};
#endif