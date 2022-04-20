#ifndef __AUDIT_RECODER_H_
#define __AUDIT_RECODER_H_
#include <fstream>
#include <memory>
#include <mutex>
#include <string>


#include "AuditEventInterface.hpp"
#include "AuditEventLsaDatabase.hpp"
#include "AuditEventNeighbor.hpp"
#include "AuditEventPacketIn.hpp"
#include "AuditEventPacketOut.hpp"
#include "AuditEventRegister.hpp"

#define MAXLOGITEM 2
class AuditRecoder {
   public:
    AuditRecoder(int maxLogItem);
    AuditRecoder(int maxLogItem,std::string outputPath);
    AuditRecoder(const AuditRecoder& a) = delete;

    void insertAuditLog(const AuditEventInterface& a);

   private:
    int maxLogItem=1000; 
    int fileCount = 0;
    int logCount = 0;
    std::string logPath = "";
    std::fstream out;
    std::mutex lock;
};
#endif