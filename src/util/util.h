#ifndef __UTIL_H_
#define __UTIL_H_
#include <iostream>
#include <string>
#include <vector>
#define htonll(x)    \
    ((1 == htonl(1)) \
         ? (x)       \
         : ((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x)    \
    ((1 == ntohl(1)) \
         ? (x)       \
         : ((uint64_t)ntohl((x)&0xFFFFFFFF) << 32) | ntohl((x) >> 32))
std::vector<std::string> split(std::string s, std::string sep);
#endif