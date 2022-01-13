#ifndef __MACADDRESS_H_
#define __MACADDRESS_H_
#include <cstdint>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

#include "ioc.h"
#include "util/log/Logger.h"
#include "util/util.h"

struct MacAddress {
    uint8_t addr[6];

    /**
     * @brief convert a string to MacAddress
     * @param address mac address string like 'd2:d4:12:9e:e3:54'
     */
    MacAddress(std::string address);

    /**
     * @brief default constructor, mac addr will be initialized as broadcast
     * address;
     */
    MacAddress();

    std::string toString() const;

    bool operator==(const MacAddress o) const;

} __attribute__((__packed__));

#endif