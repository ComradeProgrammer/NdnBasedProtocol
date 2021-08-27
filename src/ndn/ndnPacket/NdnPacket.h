#ifndef __NDNPACKET_H_
#define __NDNPACKET_H_
#include <memory>
#include <string>
#include <unordered_map>

#include "util/declaration.h"
#include "util/ndntlv/TlvObject.h"
#include "util/ndntlv/TlvObjectList.h"
// Abstract class, representing a ndn packet
class NdnPacket {
   public:
    // getter of name attribute
    std::string getName() { return name; }
    // setter of name attribute
    void setName(std::string _name) { name = _name; }

    // getter of attribute packetType
    uint32_t getPacketType() { return packetType; }
    // setter of attribute packetType
    void setPacketType(uint32_t _packetType) { packetType = _packetType; }
    // PURE VITURAL FUNTION encode this packet into binary data in tlv format
    // for fufilling ethernet packet
    virtual std::pair<int, std::unique_ptr<char[]>> encode() = 0;

    static std::shared_ptr<NdnPacket> decode(
        const char* data, std::shared_ptr<Logger> _logger = nullptr);

   protected:
    // when decoding name in tlv, we chose to simplify it into a 1-level string
    // instead of nested component in offical ndn
    std::string name;
    uint32_t packetType;

    virtual std::vector<TlvObject> encodeIntoTlvObjectArray() = 0;
};

#endif