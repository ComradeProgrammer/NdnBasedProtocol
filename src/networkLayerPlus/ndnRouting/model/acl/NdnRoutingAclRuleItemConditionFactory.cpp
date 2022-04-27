#include "NdnRoutingAclRuleItemConditionFactory.h"
using namespace std;

vector<shared_ptr<AclRuleItemCondition<NdnRoutingAclData>>> NdnRoutingAclRuleItemConditionFactory::parse(string s) {
    vector<shared_ptr<AclRuleItemCondition<NdnRoutingAclData>>> res;
    vector<string> tmp = split(s, " ");
    vector<string> input;
    for (string s : tmp) {
        if (s != "") {
            input.push_back(s);
        }
    }

    int x = 0;
    while (x < input.size()) {
        shared_ptr<AclRuleItemCondition<NdnRoutingAclData>> condition;
        if (input[x] == "interface") {
            condition = make_shared<NdnRoutingAclRuleItemConditionIntf>();

        } else if (input[x] == "macAddress") {
            condition = make_shared<NdnRoutingAclRuleItemConditionMacAddress>();
        } else if (input[x] == "packetName") {
            condition = make_shared<NdnRoutingAclRuleItemConditionPacketName>();
        } else if (input[x] == "packetType") {
            condition = make_shared<NdnRoutingAclRuleItemConditionPacketType>();
        } else if (input[x] == "routerID") {
            condition = make_shared<NdnRoutingAclRuleItemConditiionRouterID>();
        } else {
            return {};
        }

        bool ok = condition->parseRule(input, x);
        if (!ok) {
            return {};
        }
        res.push_back(condition);
    }
    return res;
}
