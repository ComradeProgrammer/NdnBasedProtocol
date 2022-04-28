#include "NdnRoutingAclRuleItemConditionIntf.h"
using namespace std;
bool NdnRoutingAclRuleItemConditionIntf::parseRule(vector<string> line, int& x) {
    int pos = x;
    if (pos >= line.size()) {
        return false;
    }
    if (line[pos] != "interface") {
        return false;
    }
    pos++;
    if (pos >= line.size()) {
        return false;
    }
    string num = line[pos];
    // whether it is a number
    for (char c : num) {
        if (!isdigit(c)) {
            return false;
        }
    }
    interfaceIndex = atoi(num.c_str());
    pos++;
    x = pos;
    return true;
}

bool NdnRoutingAclRuleItemConditionIntf::match(NdnRoutingAclData* item) {
    if (item == nullptr) {
        return false;
    }
    return item->interfaceIndex == interfaceIndex;
}