#ifndef __ACL_RULE_ITEM_CONDITION_H_
#define __ACL_RULE_ITEM_CONDITION_H_
#include <string>
#include <vector>
template<typename AclData>
class AclRuleItemCondition {
   public:
    /**
     * @brief will try to parse an ACL item from a string segments of a line
     * If the parse was successful, x will be removed to new position for the following match;
     * If the parse was not successful , x will be remained unchanged
     *
     * @param line string segments of a line of acl rule
     * @param x current position for match in this line
     * @return whether parse was successful
     */
    virtual bool parseRule(std::vector<std::string> line, int& x) = 0;
    virtual bool checkValidity(AclData* item)=0;
};
#endif