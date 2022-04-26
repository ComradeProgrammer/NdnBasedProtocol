#include "NdnRoutingAclRuleItemConditionRouterID.h"
using namespace std;

bool NdnRoutingAclRuleItemConditiionRouterID::parseRule(std::vector<std::string> line, int& x) {
    int pos = x;
    if (pos >= line.size()) {
        return false;
    }
    if (line[pos] != "routerID") {
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
    routerID = atoRID(num);
    pos++;

    x = pos;
    return true;
}
bool NdnRoutingAclRuleItemConditiionRouterID::checkValidity(NdnRoutingAclData* item) { return true; }
