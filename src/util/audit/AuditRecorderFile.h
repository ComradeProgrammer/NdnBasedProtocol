#ifndef __AUDIT_RECODER_FILE_H_
#define __AUDIT_RECODER_FILE_H_
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

#include "AuditEventInterface.hpp"
#include "AuditRecorderInterface.h"

class AuditRecorderFile : public AuditRecorderInterface {
   public:
    AuditRecorderFile(int maxLogItem);
    AuditRecorderFile(int maxLogItem, std::string outputPath);
    AuditRecorderFile(const AuditRecorderFile& a) = delete;

    virtual void insertAuditLog(const AuditEventInterface& a) override;
    virtual void setLopPath(std::string path) override {
        logPath = path;
        if (logPath[logPath.size() - 1] != '/') {
            logPath += "/";
        }
    }
    virtual void init() override;

   private:
    int maxLogItem = 1000;
    int fileCount = 0;
    int logCount = 0;
    std::string logPath = "";
    std::fstream out;
    std::mutex lock;
};
#endif