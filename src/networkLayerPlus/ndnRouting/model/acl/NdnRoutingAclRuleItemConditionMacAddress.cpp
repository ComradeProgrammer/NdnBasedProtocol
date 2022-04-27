#include "NdnRoutingAclRuleItemConditionMacAddress.h"
using namespace std;
bool NdnRoutingAclRuleItemConditionMacAddress::parseRule(vector<string> line, int& x) {
    int pos = x;
    if (pos >= line.size()) {
        return false;
    }
    if (line[pos] != "macAddress") {
        return false;
    }

    pos++;
    if (pos >= line.size()) {
        return false;
    }
    smatch result;
    regex pattern("^[0-9a-f][0-9a-f]:[0-9a-f][0-9a-f]:[0-9a-f][0-9a-f]:[0-9a-f][0-9a-f]:[0-9a-f][0-9a-f]:[0-9a-f][0-9a-f]$");
    if (!regex_match(line[pos], result, pattern)) {
        return false;
    }
    address = MacAddress(line[pos]);
    pos++;
    x = pos;
    return true;
}

bool NdnRoutingAclRuleItemConditionMacAddress::checkValidity(NdnRoutingAclData* item) {
    if (item == nullptr) {
        return false;
    }
    return item->sourceMacAddress == address;
}