#ifndef __AUDIT_EVENT_LSA_DATABASE_H_
#define __AUDIT_EVENT_LSA_DATABASE_H_
#include <string>

#include "util/audit/AuditEventInterface.hpp"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"

class AuditEventLsaDatabase : public AuditEventInterface {
   public:
    std::string time;
    RouterID sourceRouter;
    std::string lsaType;
    int sequenceNum;
    std::string operationType;
    nlohmann::json lsaContent;

    AuditEventLsaDatabase(std::string _time, RouterID _sourceRouter, std::string _lsaType, int _sequenceNum, std::string _operationType,
                          nlohmann::json _lsaContent)
        : time(_time), sourceRouter(_sourceRouter), lsaType(_lsaType), sequenceNum(_sequenceNum), operationType(_operationType), lsaContent(_lsaContent) {
        eventType = AuditEventInterface::EVENT_LSADATABASE;
    }

    virtual nlohmann::json marshal() const override{
        nlohmann::json j;
        j["eventType"]=eventType;
        j["time"] = time;
        j["sourceRouter"] = sourceRouter;
        j["lsaType"] = lsaType;
        j["sequenceNum"] = sequenceNum;
        j["operationType"] = operationType;
        j["lsaContent"] = lsaContent;
        return j;
    }

   public:
    // for lsaType
    static constexpr const char* ADJ = "ADJ";
    static constexpr const char* RCH = "RCH";
    // for operationType
    static constexpr const char* INSERT = "INSERT";
    static constexpr const char* DELETE = "DELETE";
};
#endif