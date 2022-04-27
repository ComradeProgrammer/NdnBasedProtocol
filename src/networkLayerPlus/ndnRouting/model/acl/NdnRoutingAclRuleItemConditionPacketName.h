#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_PACKET_NAME_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_PACKET_NAME_H_
#include <regex>
#include <string>

#include "NdnRoutingAclData.h"
#include "util/acl/AclRuleItemCondition.h"
class NdnRoutingAclRuleItemConditionPacketName : public AclRuleItemCondition<NdnRoutingAclData> {
   public:
    virtual bool parseRule(std::vector<std::string> line, int& x) override;
    virtual bool checkValidity(NdnRoutingAclData* item) override;

   private:
    std::string reg;
};
#endif