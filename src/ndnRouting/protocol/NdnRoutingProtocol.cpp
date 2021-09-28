#include "NdnRoutingProtocol.h"
using namespace std;
shared_ptr<NdnRoutingProtocol>NdnRoutingProtocol::protocol=nullptr;
mutex NdnRoutingProtocol::staticLock;

shared_ptr<NdnRoutingProtocol>NdnRoutingProtocol::getNdnRoutingProtocol(shared_ptr<Logger> _logger = nullptr){
    lock_guard<mutex>lockFunction(staticLock);
    if(protocol==nullptr){
        protocol=shared_ptr<NdnRoutingProtocol>(new NdnRoutingProtocol(_logger));
    }
    return protocol;
}

NdnRoutingProtocol::NdnRoutingProtocol(shared_ptr<Logger> _logger = nullptr){
    logger=Logger::getDefaultLoggerIfNull(_logger);
}

void NdnRoutingProtocol::initialize(){
    //1.establish interface data structure

}