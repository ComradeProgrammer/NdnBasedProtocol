#ifndef __ACL_RULE_ITEM_CONDITION_FACTORY_H_
#define __ACL_RULE_ITEM_CONDITION_FACTORY_H_
#include <memory>

#include "AclRuleItemCondition.h"
template <typename AclData>
class AclRuleItemConditionFactory {
   public:
    virtual std::vector<std::shared_ptr<AclRuleItemCondition<AclData>>> parse(std::string s) = 0;
};

#endif
