#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_H_
#include"util/acl/AclRuleItemCondition.h"
#include"NdnRoutingAclData.h"
class NdnRoutingAclRuleItemCondition: public AclRuleItemCondition<NdnRoutingAclData>{
    public:

    virtual bool parseRule(std::vector<std::string> line, int& x) override{
        return true;
    }
    virtual bool checkValidity(NdnRoutingAclData* item)override{

    }
};
#endif