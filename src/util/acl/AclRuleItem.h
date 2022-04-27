#ifndef __ACL_RULE_ITEM_H_
#define __ACL_RULE_ITEM_H_
#include <string>
#include <vector>

#include "AclRuleItemCondition.h"
#include "AclRuleItemConditionFactory.h"
/*
rule 5 deny/permit conditions.....
*/
template <typename AclData>
class AclRuleItem {
   public:
    AclRuleItem(std::shared_ptr<AclRuleItemConditionFactory<AclData>> _factory) { factory = _factory; }

    bool parse(std::string line) {
        vector<string> splits = split(line, " ");
        int ptr = 0;
        while (ptr < splits.size() && splits[ptr] == "") {
            ptr++;
        }
        if (ptr == splits.size() || splits[ptr] != "rule") {
            return false;
        }
        ptr++;

        while (ptr < splits.size() && splits[ptr] == "") {
            ptr++;
        }
        if (ptr == splits.size() || !isNumber(splits[ptr])) {
            return false;
        }
        ruleID = atoi(splits[ptr].c_str());
        ptr++;

        while (ptr < splits.size() && splits[ptr] == "") {
            ptr++;
        }
        if (ptr == splits.size() || (splits[ptr] != "deny" && splits[ptr] != "permit")) {
            return false;
        }
        permit = splits[ptr] == "deny" ? false : true;
        ptr++;

        for (int i = 0; i < ptr; i++) {
            splits.erase(splits.begin());
        }
        string newLine = join(splits, " ");

        conditions = factory->parse(newLine);
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

   protected:
    std::vector<std::shared_ptr<AclRuleItemCondition<AclData>>> conditions;
    std::shared_ptr<AclRuleItemConditionFactory<AclData>> factory;
    int ruleID;
    bool permit = false;
};

#endif