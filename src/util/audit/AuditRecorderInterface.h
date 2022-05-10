#ifndef __AUDIT_RECORDER_INTERFACE_H_
#define __AUDIT_RECORDER_INTERFACE_H_
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

#include "AuditEventInterface.hpp"

class AuditRecorderInterface {
   public:
    virtual void insertAuditLog(const AuditEventInterface& a) = 0;
    virtual void setLopPath(std::string path) = 0;
    virtual void init() = 0;
};
#endif