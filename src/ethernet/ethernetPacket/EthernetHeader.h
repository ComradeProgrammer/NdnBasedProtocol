#ifndef __ETHERNETHEADER_H_
#define __ETHERNETHEADER_H_
#include "MacAddress.h"
struct EthernetHeader {
    MacAddress destination;
    MacAddress source;
    uint16_t type;
} __attribute__((__packed__));
#endif