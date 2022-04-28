#ifndef __NDN_ROUTING_ACL_H_
#define __NDN_ROUTING_ACL_H_
#include "NdnRoutingAclData.h"
#include "NdnRoutingAclRuleItem.h"
#include "NdnRoutingAclRuleItemConditionFactory.h"
#include "ioc.h"
#include "util/acl/Acl.h"
#include "util/acl/AclRuleItem.h"
/*
acl rule format:
rule 5 deny/permit conditions
...
rule 6 permit all


conditions= [flag contents]  [flag contents]...

flags & contents:
interface <interfaceid>
macAddress <macAddress>
packetName <regex>
packetType <packetKind> <packetType>
routerID <routerID>


example:
rule 1 deny interface 1 macAddress ff:ff:ff:ff:ff:ff packetName /name/.* packetName dd interest routerID 46546815341564231

*/
class NdnRoutingAcl : public Acl<NdnRoutingAclData> {
   public:
    virtual void parseFile(std::string fileName) override;
    virtual void parseString(std::string str) override;
};
#endif