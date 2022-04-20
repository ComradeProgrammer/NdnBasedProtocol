#include"iostream"
#include"util/audit/AuditRecoder.h"
#include"util/util.h"
using namespace std;

int main(){
    AuditRecoder recorder(2,"auditfolder/");
    AuditEventRegister event1={
        getCurrentTime(),
        1,2,3,AuditEventRegister::REGISTER,
        true,
    };
    for(int i=0;i<10;i++){
        recorder.insertAuditLog(event1);
    }

}