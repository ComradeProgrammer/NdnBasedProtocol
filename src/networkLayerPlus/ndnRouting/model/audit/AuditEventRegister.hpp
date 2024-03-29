#ifndef __AUDIT_EVENT_REGISTER_H_
#define __AUDIT_EVENT_REGISTER_H_
#include "util/audit/AuditEventInterface.hpp"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
struct AuditEventRegister : public AuditEventInterface {
   public:
    std::string time;
    RouterID sonRouter;
    RouterID rootRouter;
    RouterID parentRouter;
    std::string registerType;

    AuditEventRegister(std::string _time, RouterID _sonRouter, RouterID _rootRouter, RouterID _parentRouter, std::string _registerType)
        : time(_time), sonRouter(_sonRouter), rootRouter(_rootRouter), parentRouter(_parentRouter), registerType(_registerType){
        eventType = EVENT_REGISTER;
    }

    virtual nlohmann::json marshal() const override{
        nlohmann::json j;
        j["eventType"]=eventType;
        j["time"] = time;
        j["sonRouter"] = sonRouter;
        j["rootRouter"] = rootRouter;
        j["parentRouter"] = parentRouter;
        j["registerType"] = registerType;
        return j;
    }

   public:
    static constexpr const char* DEREGISTER = "DEREGISTER";
    static constexpr const char* REGISTER = "REGISTER";
};
#endif