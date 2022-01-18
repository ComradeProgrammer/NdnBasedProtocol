#ifndef __PACKETCOMMON_H_
#define __PACKETCOMMON_H_
#include <string>
#include "util/util.h"
#include "util/json/json.hpp"
#include "util/printable/Printable.h"
//typedef int RouterID;
typedef uint64_t RouterID;

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

#endif