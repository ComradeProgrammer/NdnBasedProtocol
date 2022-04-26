#include "NdnRoutingAclRuleItemConditionPacketName.h"
using namespace std;
bool NdnRoutingAclRuleItemConditionPacketName::parseRule(vector<string> line, int& x) {
    int pos = x;
    if (pos >= line.size()) {
        return false;
    }
    if (line[pos] != "packetName") {
        return false;
    }
    pos++;
    if (pos >= line.size()) {
        return false;
    }
    reg = line[pos];
    pos++;
    x = pos;
    return true;
}

bool NdnRoutingAclRuleItemConditionPacketName::checkValidity(NdnRoutingAclData* item) { return true; }