#ifndef __ACL_RULE_ITEM_H_
#define __ACL_RULE_ITEM_H_
#include <string>
#include <vector>

#include "AclRuleItemCondition.h"
#include "AclRuleItemConditionFactory.h"
template <typename AclData>
class AclRuleItem {
   public:
    AclRuleItem(std::shared_ptr<AclRuleItemConditionFactory<AclData>> _factory){factory=_factory;}

    bool parse(std::string line) {
        conditions = factory->parse(line);
        return conditions.size() != 0;
    }
    
    bool checkValidity(AclData* data) {
        for (int i = 0; i < conditions.size(); i++) {
            if (conditions[i]->checkValidity(data) == false) {
                return false;
            }
        }
        return true;
    }

   private:
    std::vector<std::string> segments;
    std::vector<std::shared_ptr<AclRuleItemCondition<AclData>>> conditions;
    std::shared_ptr<AclRuleItemConditionFactory<AclData>> factory;
};

#endif