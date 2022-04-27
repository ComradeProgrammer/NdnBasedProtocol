#include "NdnRoutingAclRuleItemConditionPacketType.h"
using namespace std;

bool NdnRoutingAclRuleItemConditionPacketType::parseRule(vector<string> line, int& x) {
    int pos = x;
    if (pos >= line.size()) {
        return false;
    }
    if (line[pos] != "packetType") {
        return false;
    }

    pos++;
    if (pos >= line.size()) {
        return false;
    }

    if (line[pos] == "hello") {
        packetKind = HELLO;
    } else if (line[pos] == "dd") {
        packetKind = DD;
    } else if (line[pos] == "lsa") {
        packetKind = LSA;
    } else if (line[pos] == "info") {
        packetKind = INFO;
    } else if (line[pos] == "register") {
        packetKind = REGISTER;
    } else if (line[pos] == "deregister") {
        packetKind = DEREGISTER;
    } else {
        return false;
    }

    pos++;
    if (pos >= line.size()) {
        return false;
    }

    if (line[pos] == "interest") {
        packetType = INTEREST;
    } else if (line[pos] == "data") {
        packetType = DATA;
    } else {
        return false;
    }
    pos++;
    x = pos;
    return true;
}
bool NdnRoutingAclRuleItemConditionPacketType::checkValidity(NdnRoutingAclData* item) { return true; }