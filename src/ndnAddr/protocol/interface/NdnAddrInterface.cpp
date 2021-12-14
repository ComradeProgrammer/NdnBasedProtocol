#include"NdnAddrInterface.h"
NdnAddrInterface::NdnAddrInterface(NIC nic, std::shared_ptr<Logger> _logger){
    logger=Logger::getDefaultLoggerIfNull(_logger);
}

void NdnAddrInterface::onEventHappen(int interfaceID, NICEvent event){
    
}