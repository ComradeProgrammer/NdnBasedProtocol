#ifndef __ETHERNETHEADER_H_
#define __ETHERNETHEADER_H_
#include <arpa/inet.h>

#include "MacAddress.h"
class EthernetHeader {
    private:
    MacAddress destination;
    MacAddress source;
    uint16_t type;//stores in network's byte order(big endian)

    public:
    //getter and setters

    MacAddress getDestinationMacAddress(){return destination;}
    MacAddress getSourceMacAddress(){return source;}
    void setDestionationMacAddress(MacAddress dest){destination=dest;}
    void setSourceMacAddress(MacAddress src){source=src;}
    /**
     * @brief getter method of 'type'
     * @return type in machine's byte order
    */
    uint16_t getType(){return ntohs(type);}
    /**
     * @brief setter method of 'type',parameter will be converted into network's byte order 
     * @param t  type in machine's byte order
    */
    void setType(uint16_t t){type=htons(t);}

} __attribute__((__packed__));
#endif