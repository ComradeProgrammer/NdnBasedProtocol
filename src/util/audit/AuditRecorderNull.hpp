#ifndef __AUDIT_RECORDER_NULL_HPP_
#define __AUDIT_RECORDER_NULL_HPP_
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

#include "AuditRecorderInterface.h"

class AuditRecorderNull : public AuditRecorderInterface {
   public:
    virtual void insertAuditLog(const AuditEventInterface& a) override {}
    virtual void setLopPath(std::string path) override {}
    virtual void init() override {}
};
#endif