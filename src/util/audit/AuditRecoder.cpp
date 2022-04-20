#include "AuditRecoder.h"
using namespace std;
AuditRecoder::AuditRecoder(int _maxLogItem) {
    maxLogItem=_maxLogItem;
    string filename = to_string(fileCount) + "_audit.log";
    cout<<filename<<endl;
    out.open(filename, ios::out);
}

AuditRecoder::AuditRecoder(int _maxLogItem,string outputPath) {
    maxLogItem=_maxLogItem;
    logPath = outputPath;
    if (logPath[logPath.size() - 1] != '/') {
        logPath += "/";
    }
    string filename = logPath + to_string(fileCount) + "_audit.log";
    cout<<filename<<endl;
    out.open(filename, ios::out);
}

void AuditRecoder::insertAuditLog(const AuditEventInterface& a) {
    lock_guard<mutex>l(lock);
    out<<a.marshal().dump()<<endl;
    logCount++;
    if(logCount==maxLogItem){
        logCount=0;
        out.close();
        fileCount++;
        string filename = logPath + to_string(fileCount) + "_audit.log";
        out.open(filename, ios::out);
    }
}