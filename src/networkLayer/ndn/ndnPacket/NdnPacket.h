#ifndef __NDNPACKET_H_
#define __NDNPACKET_H_
#include <memory>
#include <string>
#include <unordered_map>

#include "linkLayer/MacAddress.h"
#include "util/declaration.h"
#include "tlv/TlvObject.h"
#include "tlv/TlvObjectList.h"
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

    // PURE VITURAL FUNTION convert this packet to a readable string
    virtual std::string toString() = 0;

    static std::shared_ptr<NdnPacket> decode(const char* data, std::shared_ptr<Logger> _logger = nullptr);

    // only used by ndn layer, will not be encoded
    void setPreferedInterfaces(std::vector<std::pair<int, MacAddress>> interfaces) { preferedInterfaces = interfaces; }
    std::vector<std::pair<int, MacAddress>> getPreferedInterfaces() { return preferedInterfaces; }
    bool hasPreferedInterfaces() { return preferedInterfaces.size() != 0; }

   protected:
    // when decoding name in tlv, we chose to simplify it into a 1-level string
    // instead of nested component in offical ndn
    std::string name;
    uint32_t packetType;
    // only used by ndn layer, will not be encoded
    std::vector<std::pair<int, MacAddress>> preferedInterfaces;

    virtual std::vector<TlvObject> encodeIntoTlvObjectArray() = 0;
};

#endif