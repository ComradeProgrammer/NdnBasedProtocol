#ifndef __PACKETCOMMON_H_
#define __PACKETCOMMON_H_
#include <string>

#include "util/json/json.hpp"
#include "util/printable/Printable.h"
#include "util/util.h"
// typedef int RouterID;
typedef uint64_t RouterID;
using namespace std;
enum LinkStateType { ADJ = 0, RCH = 1 };
inline std::string getNameForLinkStateType(LinkStateType t) {
    if (t == LinkStateType::ADJ) {
        return "ADJ";
    }
    if (t == LinkStateType::RCH) {
        return "RCH";
    }
    return "";
}

inline RouterID atoRID(std::string s) {
    if (sizeof(RouterID) == 4) {
        return atoll(s.c_str());
    } else if (sizeof(RouterID) == 8) {
        if (s.size() < 5) {
            return atoll(s.c_str());
        }
        string lower = s.substr(s.size() - 4, 4);
        string upper = s.substr(0, s.size() - 4);
        uint64_t upperNum = atoll(upper.c_str());
        uint64_t lowerNum = atoll(lower.c_str());
        return upperNum * 10000 + lowerNum;
    }
}
#endif