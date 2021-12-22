#include "Logger.h"
#include "TerminalLogger.h"
using namespace std;
shared_ptr<Logger>Logger::logger;
mutex Logger::staticLock;

void Logger::init(shared_ptr<Logger>_logger){
    lock_guard<mutex>lockFunction(staticLock);
    logger=_logger;
    if(logger==nullptr){
        logger=make_shared<TerminalLogger>();
    }
}
shared_ptr<Logger> Logger::getDefaultLogger(){
    lock_guard<mutex>lockFunction(staticLock);
    return  logger;
}