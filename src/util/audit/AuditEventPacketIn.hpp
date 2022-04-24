#ifndef __AUDIT_EVENT_PACKET_IN_H_
#define __AUDIT_EVENT_PACKET_IN_H_
#include <string>

#include "AuditEventInterface.hpp"
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
class AuditEventPacketIn : public AuditEventInterface {
   public:
    std::string time;
    int interface;
    MacAddress sourceMac;
    RouterID sourceRouter;
    std::string ndnPacketType;      // interest or data
    std::string routingPacketType;  // hello dd lsa or register?
    std::string packetName;
    nlohmann::json packageContent;

    AuditEventPacketIn(std::string _time, int _interface, MacAddress _sourceMac, RouterID _sourceRouter, std::string _ndnPacketType,
                       std::string _routingPacketType, std::string _packetName, nlohmann::json _packageContent)
        : time(_time),
          interface(_interface),
          sourceMac(_sourceMac),
          sourceRouter(_sourceRouter),
          ndnPacketType(_ndnPacketType),
          routingPacketType(_routingPacketType),
          packetName(_packetName),
          packageContent(_packageContent) {
        eventType = EVENT_PACKET_IN;
    }

    virtual nlohmann::json marshal() const override{
        nlohmann::json j;
        j["eventType"]=eventType;
        j["time"] = time;
        j["interface"] = interface;
        j["sourceMac"] = sourceMac.toString();
        j["sourceRouter"] = sourceRouter;
        j["ndnPacketType"] = ndnPacketType;
        j["routingPacketType"] = routingPacketType;
        j["packetName"] = packetName;
        j["packageContent"] = packageContent;
        return j;
    }
};
#endif