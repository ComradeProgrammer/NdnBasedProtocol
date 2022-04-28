#ifndef __ACL_H_
#define __ACL_H_
#include "AclRuleItem.h"
#include "AclRuleItemCondition.h"
template <typename AclData>
class Acl {
   public:
    virtual void parseFile(std::string fileName) = 0;
    virtual void parseString(std::string str) = 0;
    void setDefaultDenyAll(bool val) { defaultDenyAll = val; }

    bool match(AclData* data) {
        for (auto item : items) {
            if (item->match(data)) {
                return item->getPermit();
            }
        }
        return defaultDenyAll ? false : true;
    }

   protected:
    std::vector<std::shared_ptr<AclRuleItem<AclData>>> items;
    bool defaultDenyAll = false;
};
#endif