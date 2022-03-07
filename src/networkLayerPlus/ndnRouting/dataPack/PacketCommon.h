#ifndef __PACKETCOMMON_H_
#define __PACKETCOMMON_H_
#include <string>

#include "util/json/json.hpp"
#include "util/printable/Printable.h"
#include "util/util.h"
// typedef int RouterID;
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

inline RouterID atoRID(std::string s){
    if(sizeof(RouterID)==4){
        return atoi(s.c_str());
    }else if(sizeof(RouterID)==8){
        return atoll(s.c_str());
    }
}
#endif