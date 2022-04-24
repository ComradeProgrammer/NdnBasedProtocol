#ifndef __AUDIT_EVENT_PACKET_OUT_H_
#define __AUDIT_EVENT_PACKET_OUT_H_
#include <string>

#include "AuditEventInterface.hpp"
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
class AuditEventPacketOut : public AuditEventInterface {
   public:
    std::string time;
    int interface;
    MacAddress targetMac;
    RouterID targetRouter;
    std::string ndnPacketType;      // interest or data
    std::string routingPacketType;  // hello dd lsa or register?
    std::string packetName;
    nlohmann::json packageContent;

    AuditEventPacketOut(std::string _time, int _interface, MacAddress _targetMac, RouterID _targetRouter, std::string _ndnPacketType,
                        std::string _routingPacketType, std::string _packetName, nlohmann::json _packageContent)
        : time(_time),
          interface(_interface),
          targetMac(_targetMac),
          targetRouter(_targetRouter),
          ndnPacketType(_ndnPacketType),
          routingPacketType(_routingPacketType),
          packetName(_packetName),
          packageContent(_packageContent) {
        eventType = EVENT_PACKET_OUT;
    }

    virtual nlohmann::json marshal() const override {
        nlohmann::json j;
        j["eventType"]=eventType;
        j["time"] = time;
        j["interface"] = interface;
        j["targetMac"] = targetMac.toString();
        j["targetRouter"] = targetRouter;
        j["ndnPacketType"] = ndnPacketType;
        j["routingPacketType"] = routingPacketType;
        j["packetName"] = packetName;
        j["packageContent"] = packageContent;
        return j;
    }
};
#endif