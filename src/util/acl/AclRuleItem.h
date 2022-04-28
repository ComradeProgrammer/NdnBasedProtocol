#ifndef __ACL_RULE_ITEM_H_
#define __ACL_RULE_ITEM_H_
#include <string>
#include <vector>

#include "AclRuleItemCondition.h"
#include "AclRuleItemConditionFactory.h"
#include "util/util.h"
/*
acl rule format:
rule 5 deny/permit conditions.....
rule 6 permit all
*/
template <typename AclData>
class AclRuleItem {
   public:
    AclRuleItem(std::shared_ptr<AclRuleItemConditionFactory<AclData>> _factory) { factory = _factory; }

    virtual bool parse(std::string line) {
        std::vector<std::string> splits = split(line, " ");
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

        while (ptr < splits.size() && splits[ptr] == "") {
            ptr++;
        }
        if (ptr == splits.size()) {
            return false;
        } else if (splits[ptr] == "all") {
            all = true;
            return true;
        }

        for (int i = 0; i < ptr; i++) {
            splits.erase(splits.begin());
        }

        std::string newLine = join(splits, " ");

        conditions = factory->parse(newLine);
        return conditions.size() != 0;
    }
    /**
     * @brief return whether this data matches the rule.
     */
    virtual bool match(AclData* data) {
        if (all) {
            return true;
        }
        for (int i = 0; i < conditions.size(); i++) {
            if (conditions[i]->match(data) == false) {
                return false;
            }
        }
        return true;
    }
    /**
     * @brief return whether this rule should be permitted or rejected when matched
     */
    bool getPermit() { return permit; }

   protected:
    std::vector<std::shared_ptr<AclRuleItemCondition<AclData>>> conditions;
    std::shared_ptr<AclRuleItemConditionFactory<AclData>> factory;
    int ruleID;
    bool permit = false;

    bool all = false;
};

#endif