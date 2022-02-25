#ifndef __UTIL_H_
#define __UTIL_H_
#include <arpa/inet.h>
#include <sys/time.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class MacAddress;
#define htonll(x) ((1 == htonl(1)) ? (x) : ((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1 == ntohl(1)) ? (x) : ((uint64_t)ntohl((x)&0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#define htonu128(x) ((1 == htonl(1)) ? (x) : ((__uint128_t)htonll((x)&0xFFFFFFFFFFFFFFFF) << 64) | htonll((x) >> 64))
#define ntohu128(x) ((1 == ntohl(1)) ? (x) : ((__uint128_t)ntohll((x)&0xFFFFFFFFFFFFFFFF) << 64) | ntohll((x) >> 64))
std::vector<std::string> split(std::string s, std::string sep);
std::string intVectorToString(std::vector<int> input);
std::string intMacAddressVectorToString(std::vector<std::pair<int, MacAddress>> input);
std::string getCurrentTime();

template <typename T>
T ntoh(T t) {
    int size = sizeof(T);
    if (size == 1) {
        return t;
    } else if (size == 2) {
        return ntohs((uint16_t)t);
    } else if (size == 4) {
        return ntohl((uint32_t)t);
    } else if (size == 8) {
        return ntohll((uint64_t)t);
    } else if (size == 16) {
        return ntohu128((__uint128_t)t);
    }
    return -1;
}

template <typename T>
T hton(T t) {
    return ntoh(t);
}

std::string hexString(const void* data, unsigned int length);

#endif