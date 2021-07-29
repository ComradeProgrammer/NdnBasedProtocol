#ifndef __ETHERNETPACKET_H_
#define __ETHERNETPACKET_H_
#include <cstring>

#include "EthernetHeader.h"
#include "util/log/Logger.h"
class EthernetPacket {
   public:
    /**
     * @brief construct a packet with specified header,data,dataLength
     */
    EthernetPacket(EthernetHeader header, char* data, int dataLenghth,
                   std::shared_ptr<Logger> log = nullptr);
    /**
     * @brief construct packet from raw byte data
     */
    EthernetPacket(const char* rawData, int rawDataLength,
                   std::shared_ptr<Logger> log = nullptr);

    /**
     * @brief copy constructor of  EthernetPacket
     */
    EthernetPacket(const EthernetPacket& oldPacket);
    /**
     * deconstructor of EthernetPacket
     */
    ~EthernetPacket();
    /**
     * @brief Copy assignment operator of  EthernetPacket
     */
    EthernetPacket& operator=(const EthernetPacket& oldPacket);

    // getter of header attribut
    EthernetHeader getHeader() { return header; }

    /**
     * @brief return the pointer of data part of the packet. You MUST NOT modify
     * the data pointed by this pointer. Make a copy if you want to alter this
     * data.
     */
    const char* getData() { return data; }

    // return the total size of packet
    int getPacketSize() { return dataSize + 14; }

   protected:
    EthernetHeader header;
    char* data = nullptr;
    int dataSize = 0;
    std::shared_ptr<Logger> logger = nullptr;
};
#endif