#ifndef __MACADDRESS_H_
#define __MACADDERSS_H_
#include<cstdint>
#include<string>
#include<memory>
#include<sstream>
#include<cstdio>
#include"util/util.h"
#include"util/log/Logger.h"

struct MacAddress{
    uint8_t addr[6];

    /**
     * @brief convert a string to MacAddress
     * @param address mac address string like 'd2:d4:12:9e:e3:54'
    */
    MacAddress(std::string address,std::shared_ptr<Logger>logger=nullptr);

    std::string toString();

}__attribute__ ((__packed__));

#endif