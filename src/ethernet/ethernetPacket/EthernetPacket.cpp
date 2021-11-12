#include "EthernetPacket.h"

EthernetPacket::EthernetPacket(EthernetHeader _header, char* _data, int dataLenghth, std::shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
    header = _header;
    data = new char[dataLenghth];
    memcpy(data, _data, dataLenghth);
    dataSize = dataLenghth;
}
EthernetPacket::EthernetPacket(MacAddress destination, MacAddress source, uint16_t protocol, const char* _data,
                               int dataLenghth, std::shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
    header.setDestionationMacAddress(destination);
    header.setSourceMacAddress(source);
    header.setType(protocol);

    data = new char[dataLenghth];
    memcpy(data, _data, dataLenghth);
    dataSize = dataLenghth;
}

EthernetPacket::EthernetPacket(const char* rawData, int rawDataLength, std::shared_ptr<Logger> log) {
    logger = Logger::getDefaultLoggerIfNull(log);
    if (rawDataLength < 14) {
        logger->ERROR("packet data's length <14");
        return;
    }
    header = EthernetHeader(rawData);
    data = new char[rawDataLength - 14];
    memcpy(data, rawData + 14, rawDataLength - 14);
    dataSize = rawDataLength - 14;
}

EthernetPacket::EthernetPacket(const EthernetPacket& oldPacket) {
    header = oldPacket.header;
    data = new char[sizeof(oldPacket.data)];
    memcpy(data, oldPacket.data, sizeof(oldPacket.data));
    logger = oldPacket.logger;
    dataSize = oldPacket.dataSize;
}

EthernetPacket::~EthernetPacket() { delete[] data; }

EthernetPacket& EthernetPacket::operator=(const EthernetPacket& oldPacket) {
    delete[] data;
    header = oldPacket.header;
    data = new char[sizeof(oldPacket.data)];
    memcpy(data, oldPacket.data, sizeof(oldPacket.data));
    dataSize = oldPacket.dataSize;
    logger = oldPacket.logger;
    return *this;
}