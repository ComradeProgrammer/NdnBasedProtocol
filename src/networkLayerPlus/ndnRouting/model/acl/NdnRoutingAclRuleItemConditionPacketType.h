#ifndef __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_PACKET_TYPE_H_
#define __NDN_ROUTING_ACL_RULE_ITEM_CONDITION_PACKET_TYPE_H_
#include <string>

#include "NdnRoutingAclData.h"
#include "util/acl/AclRuleItemCondition.h"
class NdnRoutingAclRuleItemConditionPacketType : public AclRuleItemCondition<NdnRoutingAclData> {
   public:
    virtual bool parseRule(std::vector<std::string> line, int& x) override;
    virtual bool checkValidity(NdnRoutingAclData* item) override;

   private:
    PacketType packetType;
    PacketKind packetKind;
};
#endif