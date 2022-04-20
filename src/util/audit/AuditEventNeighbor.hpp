#ifndef __AUDIT_EVENT_NEIGHBOR_H_
#define __AUDIT_EVENT_NEIGHBOR_H_
#include "AuditEventInterface.hpp"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
class AuditEventNeighbor : public AuditEventInterface {
   public:
    std::string time;
    RouterID neighborID;
    std::string neighborEvent;
    std::string oldState;
    std::string newState;
    AuditEventNeighbor(std::string _time, RouterID _neighborID, std::string _neighborEvent, std::string _oldState, std::string _newState)
        : time(_time), neighborID(_neighborID), neighborEvent(_neighborEvent), oldState(_oldState), newState(_newState) {}
    virtual nlohmann::json marshal() const {
        nlohmann::json j;
        j["time"] = time;
        j["neighborID"] = neighborID;
        j["neighborEvent"] = neighborEvent;
        j["oldState"] = oldState;
        j["newState"] = newState;
        return j;
    }
};
#endif