#include "NdnRoutingAcl.h"
using namespace std;
bool isEmptyString(string s) {
    for (auto c : s) {
        if (isspace(c) || isblank(c)) {
            continue;
        }
        return false;
    }
    return true;
}

void NdnRoutingAcl::parseFile(string fileName) {
    string line;
    fstream file(fileName, ios::in);
    while (getline(file, line)) {
        if (isEmptyString(line)) {
            continue;
        }

        shared_ptr<NdnRoutingAclRuleItemConditionFactory> factory = make_shared<NdnRoutingAclRuleItemConditionFactory>();
        shared_ptr<NdnRoutingAclRuleItem> item = make_shared<NdnRoutingAclRuleItem>(factory);
        bool ok = item->parse(line);
        if (!ok) {
            cout << "unrecognized acl rule " << line << endl;
            // LOGGER->ERRORF("unrecognized acl rule %s", line);
            continue;
        }
        items.push_back(item);
    }
}

void NdnRoutingAcl::parseString(string str) {
    vector<string> lines = split(str, "\n");
    for (auto line : lines) {
        if (isEmptyString(line)) {
            continue;
        }

        shared_ptr<NdnRoutingAclRuleItemConditionFactory> factory = make_shared<NdnRoutingAclRuleItemConditionFactory>();
        shared_ptr<NdnRoutingAclRuleItem> item = make_shared<NdnRoutingAclRuleItem>(factory);
        bool ok = item->parse(line);
        if (!ok) {
            cout << "unrecognized acl rule " << line << endl;
            // LOGGER->ERRORF("unrecognized acl rule %s", line);
            continue;
        }
        items.push_back(item);
    }
}