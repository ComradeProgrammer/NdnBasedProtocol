#include "AuditRecorderFile.h"
using namespace std;
AuditRecorderFile::AuditRecorderFile(int _maxLogItem) {
    maxLogItem = _maxLogItem;
    string filename = to_string(fileCount) + "_audit.log";
}

AuditRecorderFile::AuditRecorderFile(int _maxLogItem, string outputPath) {
    maxLogItem = _maxLogItem;
    logPath = outputPath;
    if (logPath[logPath.size() - 1] != '/') {
        logPath += "/";
    }
    
}
void AuditRecorderFile::init(){
    string filename = logPath + to_string(fileCount) + "_audit.log";
    out.open(filename, ios::out);
}

void AuditRecorderFile::insertAuditLog(const AuditEventInterface& a) {
    lock_guard<mutex> l(lock);
    out << a.marshal().dump() << endl;
    logCount++;
    if (logCount == maxLogItem) {
        logCount = 0;
        out.close();
        fileCount++;
        string filename = logPath + to_string(fileCount) + "_audit.log";
        out.open(filename, ios::out);
    }
}