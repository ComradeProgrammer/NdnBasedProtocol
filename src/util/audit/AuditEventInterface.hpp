#ifndef __AUDIT_EVENT_INTERFACE_H_
#define __AUDIT_EVENT_INTERFACE_H_
#include "util/json/json.hpp"

class AuditEventInterface {
   public:
    std::string eventType;
    virtual nlohmann::json marshal() const = 0;

   public:
    // for eventType
    static constexpr const char* EVENT_PACKET_IN = "PACKET_IN";
    static constexpr const char* EVENT_PACKET_OUT = "PACKET_OUT";
    static constexpr const char* EVENT_LSADATABASE = "LSADATABASE";
    static constexpr const char* EVENT_REGISTER = "REGISTER";
    static constexpr const char* EVENT_NEIGHBOR = "NEIGHBOR";
    // for ndnPacketType
    static constexpr const char* INTEREST = "INTEREST";
    static constexpr const char* DATA = "DATA";
    // for routingPacketType
    static constexpr const char* HELLO_PACKET = "HELLO";
    static constexpr const char* DD_PACKET = "DD";
    static constexpr const char* LSA_PACKET = "LSA";
    static constexpr const char* INFO_PACKET = "INFO";
    static constexpr const char* REGISTER_PACKET = "REGISTER";
    static constexpr const char* DEREGISTER_PACKET = "DEREGISTER";
};
#endif